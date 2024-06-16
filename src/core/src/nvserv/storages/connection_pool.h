#pragma once

#include <absl/container/node_hash_map.h>

#include <deque>
#include <queue>
#include <thread>

#include "nvserv/declare.h"
#include "nvserv/exceptions.h"
#include "nvserv/global_macro.h"
#include "nvserv/headers/absl_thread.h"
#include "nvserv/storages/config.h"
#include "nvserv/storages/connection.h"
#include "nvserv/storages/declare.h"
// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

typedef ConnectionPtr (*ConnectionCreatePrimaryCallback)(StorageConfig* config);
typedef ConnectionPtr (*ConnectionCreateStandbyCallback)(StorageConfig* config);

namespace impl {
class ConnectionPoolImpl {
 public:
  explicit ConnectionPoolImpl(StorageConfig& config)
                  : acquired_(), config_(config), is_run_(false) {}

  ~ConnectionPoolImpl() {
    Stop();
  }

  void Run(const ConnectionCreatePrimaryCallback callback) {
    if (is_run_)
      return;

    if (!callback)
      throw InvalidArgException(
          "Null-reference on \"ConnectionCreateCallback callback\".");
    is_run_ = true;

    auto min_conn = config_.PoolConfig().MinConnection();
    for (size_t i = 0; i < min_conn; i++) {
      connections_.push(std::move(callback(&config_)));
      auto c = connections_.back();
      c->Open();
    }

    thread_ping_ = std::thread(&ConnectionPoolImpl::PingRunner, this);
    thread_cleanup_ = std::thread(&ConnectionPoolImpl::CleanupRunner, this);
  }

  void Wait() {
    absl::MutexLock lock(&mutex_runner_);
    if (!is_run_)
      throw ThreadException("Wait() on stopped ConnectionPool, run the "
                            "ConnectionPool.Run() before calling Wait().");
    while (is_run_) {
      cv_runner_.Wait(&mutex_runner_);
    }
  }

  void Stop() {
    {
      absl::MutexLock lock(&mutex_runner_);
      if (!is_run_)
        return;
    }

    {
      absl::MutexLock lock(&mutex_main_);
      is_run_ = false;
    }

    cv_main_.SignalAll();
    cv_cleanup_.Signal();
    cv_ping_.Signal();
    cv_runner_.Signal();

    if (thread_ping_.joinable())
      thread_ping_.join();
    if (thread_cleanup_.joinable())
      thread_cleanup_.join();

    auto size = connections_.size();
    for (size_t i = 0; i < size; i++) {
      auto element = connections_.front();
      connections_.pop();
      element->Close();
      element->Release();
      element = nullptr;
    }
  }

  ConnectionPtr Acquire() {
    absl::MutexLock lock(&mutex_main_);
    if (connections_.empty() && is_run_) {
      auto deadline_duration = absl::Seconds(
          config_.PoolConfig().MaxWaitingForConnectionAvailable().count());
      if (!cv_main_.WaitWithDeadline(&mutex_main_,
                                     absl::Now() + deadline_duration)) {
        // Timed out
        return nullptr;
      }
    }

    if (!is_run_) {
      return nullptr;
    }

    auto conn = connections_.front();
    connections_.pop();
    acquired_.emplace(conn->GetHash(), conn);
    conn->Acquire();
    return conn;
  }

  bool Return(ConnectionPtr conn) {
    if (!conn)
      return false;
    absl::MutexLock lock(&mutex_main_);
    if (!acquired_.contains(conn->GetHash()))
      return false;
    auto key = conn->GetHash();
    conn->Returned();
    connections_.push(std::move(conn));
    acquired_.erase(key);
    cv_main_.Signal();
    return true;
  }

  const bool& IsRun() const {
    return is_run_;
  }

 private:
  absl::node_hash_map<size_t, ConnectionPtr> acquired_;
  StorageConfig& config_;
  std::queue<ConnectionPtr> connections_;
  // Main mutex to handle the data
  absl::Mutex mutex_main_;

  // To hold the thread because Run will be spawned
  // from new Thread (not main Thread)
  absl::Mutex mutex_runner_;

  // To sync Cleanup routine running in Separate Thread and avoid
  // locking Acquire, Return, Stop routine
  absl::Mutex mutex_cleanup_;

  // To sync PingServer routine running in Separate Thread
  // and avoid locking Acquire, Return, Stop routine
  absl::Mutex mutex_ping_;

  absl::CondVar cv_main_;
  absl::CondVar cv_runner_;
  absl::CondVar cv_cleanup_;
  absl::CondVar cv_ping_;

  std::thread thread_cleanup_;
  std::thread thread_ping_;
  bool is_run_;

  void CleanupRunner() {
    auto deadline_duration =
        absl::Seconds(config_.PoolConfig().CleanupInterval().count());
    while (true) {
      auto state = cv_cleanup_.WaitWithDeadline(
          &mutex_cleanup_, absl::Now() + deadline_duration);
      if (!is_run_)
        break;

      absl::MutexLock lock(&mutex_main_);
      for (size_t i = connections_.size(); i > 0; --i) {
        auto element = connections_.front();
        connections_.pop();

        if (element->StandbyMode() == ConnectionStandbyMode::Standby &&
            element->IsIdle()) {
          element->Release();
        } else {
          connections_.push(std::move(element));
        }
      }
    }
  }

  void PingRunner() {
    auto deadline_duration =
        absl::Seconds(config_.PoolConfig().PingServerInterval().count());
    while (true) {
      auto state = cv_ping_.WaitWithDeadline(&mutex_ping_,
                                             absl::Now() + deadline_duration);
      if (!is_run_)
        break;

      absl::MutexLock lock(&mutex_main_);
      for (size_t i = connections_.size(); i > 0; --i) {
        auto element = connections_.front();
        connections_.pop();
        element->PingServerAsync();
        connections_.push(std::move(element));
      }
    }
  }
};
}  // namespace impl

class ConnectionPool {
 private:
 public:
  virtual ~ConnectionPool() {
    Stop();
  }

  const ConnectionPoolConfig& Config() const {
    return config_.PoolConfig();
  }

  void Run() {
    absl::MutexLock lock(&mutex_);
    if (pool_impl_ && pool_impl_->IsRun())
      return;

    thread_ = std::thread(&ConnectionPool::Runner, this);
    pool_impl_->Wait();
  }

  void Wait() {
    absl::MutexLock lock(&mutex_);
    if (!pool_impl_ || !pool_impl_->IsRun())
      return;

    if (thread_.joinable())
      thread_.join();
  }

  void Stop() {
    absl::MutexLock lock(&mutex_);
    if (!pool_impl_ || !pool_impl_->IsRun())
      return;

    pool_impl_->Stop();
  }

 protected:
  explicit ConnectionPool(StorageConfig& config) : config_(config){};

  ConnectionCreatePrimaryCallback create_primary_connection_callback_;
  ConnectionCreateStandbyCallback create_secondary_connection_callback_;
  std::unique_ptr<impl::ConnectionPoolImpl> pool_impl_;

  StorageConfig& config_;
  absl::Mutex mutex_;
  std::thread thread_;

 private:
  void Runner() {
    pool_impl_ = std::make_unique<impl::ConnectionPoolImpl>(config_);
    pool_impl_->Run(create_primary_connection_callback_);
    pool_impl_->Wait();
  }
};

NVSERV_END_NAMESPACE
#pragma once

#include <nvm/dates/datetime.h>

#include <chrono>

#include "nvserv/global_macro.h"
#include "nvserv/headers/absl_thread.h"
#include "nvserv/storages/declare.h"
#include "prepared_statement_manager.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ConnectionBase {
 public:
  // virtual ~Connection() = default;
  virtual TransactionPtr CreateTransaction() = 0;
  virtual void Open() = 0;
  virtual void Close() = 0;

  ///< Ping server to report keep-alive.
  /// Different storage server has different implementations
  /// to guarantee the connection is keep-alive
  virtual void PingServerAsync() = 0;
  virtual bool PingServer() = 0;
  virtual std::chrono::system_clock::time_point LastPing() const = 0;

  virtual ConnectionStandbyMode StandbyMode() const = 0;
  virtual const std::string& Name() const = 0;
  ///< Write state when when connection being acquired to pool
  virtual void Acquire() = 0;

  ///< Write state when when connection being returned to pool
  virtual void Returned() = 0;

  /// Return time when the Connection object was created
  virtual std::chrono::system_clock::time_point CreatedTime() const = 0;

  /// Return time when the Connection object acquired from pool
  virtual std::chrono::system_clock::time_point AcquiredTime() const = 0;

  /// Return time when the Connection object returned to pool
  virtual std::chrono::system_clock::time_point ReturnedTime() const = 0;

  /// Duration since last usage
  virtual std::chrono::seconds IdleDuration() const = 0;

  virtual std::chrono::seconds IdleAfter() const = 0;

  virtual TransactionMode SupportedTransactionMode() const = 0;

  virtual void ReportHealth() const = 0;

  virtual bool IsOpen() const = 0;

  virtual ConnectionMode Mode() const = 0;

  virtual bool IsIdle() const = 0;

  virtual void Release() = 0;

  virtual size_t GetHash() const = 0;

  virtual StorageType Type() const = 0;

  virtual const std::string& GetConnectionString() const = 0;

  virtual std::optional<std::pair<std::string, bool>> PrepareStatement(
      __NR_STRING_COMPAT_REF query) = 0;
  virtual PreparedStatementManagerPtr PreparedStatement() = 0;

 protected:
  ConnectionBase(){};
};

class Connection : public ConnectionBase {
 public:
  StorageType Type() const override {
    return type_;
  }

  const std::string& Name() const override {
    return name_;
  }

  ///< Write state when when connection being acquired to pool
  void Acquire() override {
    absl::MutexLock lock(&mutex_);
    acquired_ = nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time();
  }

  ///< Write state when when connection being returned to pool
  void Returned() override {
    absl::MutexLock lock(&mutex_);
    returned_ = nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time();
  }

  /// Return time when the Connection object was created
  std::chrono::system_clock::time_point CreatedTime() const override {
    return created_;
  }

  ///< Time when connection being acquired from pool
  std::chrono::system_clock::time_point AcquiredTime() const override {
    return acquired_;
  }

  ///< Time  when connection being acquired from pool
  std::chrono::system_clock::time_point ReturnedTime() const override {
    return returned_;
  }

  PreparedStatementManagerPtr PreparedStatement() override {
    return prepared_statement_manager_;
  }

  bool IsIdle() const override {
    auto idle = IdleDuration();
    return idle > mark_idle_after_ ? true : false;
  }

  /// Duration since last usage
  std::chrono::seconds IdleDuration() const override {
    auto now = nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time();
    return std::chrono::duration_cast<std::chrono::seconds>(now - returned_);
  }

  std::chrono::seconds IdleAfter() const override {
    return mark_idle_after_;
  }

  std::chrono::system_clock::time_point LastPing() const override {
    return last_ping_;
  }

  ConnectionStandbyMode StandbyMode() const override {
    return standby_mode_;
  }

 protected:
  explicit Connection(
      const std::string& name, StorageType type,
      ConnectionStandbyMode standby_mode,
      std::chrono::seconds mark_idle_after = std::chrono::seconds(300))
                  : ConnectionBase(),
                    name_(std::string(name)),
                    prepared_statement_manager_(
                        std::make_shared<PreparedStatementManager>()),
                    created_(nvm::dates::DateTime::UtcNow()
                                 .TzTime()
                                 ->get_sys_time()),
                    acquired_(created_),
                    returned_(created_),
                    last_ping_(created_),
                    mark_idle_after_(mark_idle_after),
                    type_(type),
                    standby_mode_(standby_mode) {}

  std::string name_;
  mutable absl::Mutex mutex_;
  PreparedStatementManagerPtr prepared_statement_manager_;
  std::chrono::system_clock::time_point created_;
  std::chrono::system_clock::time_point acquired_;
  std::chrono::system_clock::time_point returned_;
  std::chrono::system_clock::time_point last_ping_;
  std::chrono::seconds mark_idle_after_;
  StorageType type_;
  ConnectionStandbyMode standby_mode_;

  virtual void OpenImpl() = 0;
  virtual void CloseImpl() = 0;
  virtual void ReleaseImpl() = 0;
};

NVSERV_END_NAMESPACE
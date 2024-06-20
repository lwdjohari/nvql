/*
 * Copyright (c) 2024 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com>
 * Licensed to Linggawasistha Djohari under one or more contributor license
 * agreements.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

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
#include "nvserv/threads/event_loop_executor.h"
// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

typedef ConnectionPtr (*ConnectionCreatePrimaryCallback)(
    const std::string& name, StorageConfig* config);
typedef ConnectionPtr (*ConnectionCreateStandbyCallback)(
    const std::string& name, StorageConfig* config);

static constexpr std::chrono::seconds DEFAULT_CLEANUP_INTERVAL =
    std::chrono::seconds(160);
static constexpr std::chrono::seconds DEFAULT_IDLE_PING =
    std::chrono::seconds(30);
static constexpr std::chrono::seconds DEFAULT_MAX_WAITING_FOR_CONNECTION =
    std::chrono::seconds(5);

static constexpr uint16_t DEFAULT_WORKER_MINIMAL = 1;
static constexpr uint16_t DEFAULT_WORKER_MAXIMAL = 1;

class ConnectionPool {
 public:
  explicit ConnectionPool(const std::string& name, StorageConfig& config)
                  : name_(std::string(name)), config_(config){};
  virtual ~ConnectionPool() {}

  const std::string& Name() const {
    return name_;
  }
  const ConnectionPoolConfig& Config() const {
    return config_.PoolConfig();
  }

  void Run() {
    is_ready_ = false;
    RunImpl();
  }

  void SetPrimaryConnectionCallback(ConnectionCreatePrimaryCallback callback) {
    create_primary_connection_callback_ = callback;
  }

  void RemovePrimaryConnectionCallback() {
    create_primary_connection_callback_ = nullptr;
  }

  void SetStandbyConnectionCallback(ConnectionCreateStandbyCallback callback) {
    create_secondary_connection_callback_ = callback;
  }

  void RemoveStandbyConnectionCallback() {
    create_secondary_connection_callback_ = nullptr;
  }

  void Stop() {
    StopImpl();
  }

  void StopImpl() {
    absl::MutexLockMaybe lock(&mutex_main_);
    if (!is_run_)
      return;

    is_run_ = false;

    acquired_.clear();
    auto size = connections_.size();
    for (size_t i = 0; i < size; i++) {
      auto element = connections_.front();
      connections_.pop();
      element = nullptr;
    }

    // close all conections
    for (auto& conn : connection_storages_) {
      std::cout << "Close: " << conn.second->GetHash() << std::endl;
      conn.second->Close();
      conn.second.reset();
    }

    connection_storages_.clear();
  }

  ConnectionPtr Acquire() {
    absl::MutexLock lock(&mutex_main_);
    // If empty wait until connection returned at least one
    if (connections_.empty() && is_run_) {
      auto deadline =
          absl::Now() +
          absl::Seconds(
              config_.PoolConfig().MaxWaitingForConnectionAvailable().count() ==
                      0
                  ? DEFAULT_MAX_WAITING_FOR_CONNECTION.count()
                  : config_.PoolConfig()
                        .MaxWaitingForConnectionAvailable()
                        .count());

      if (!cv_main_.WaitWithDeadline(&mutex_main_, deadline)) {
        // Timed out
        return nullptr;
      }
    }

    if (!is_run_) {
      return nullptr;
    }

    auto conn = connections_.front();

    connections_.pop();
    acquired_.emplace(conn->first, conn);
    conn->second->Acquire();

    return conn->second;
  }

  bool Return(ConnectionPtr conn) {
    if (!conn)
      return false;
    absl::MutexLock lock(&mutex_main_);

    // Check if the connection was originaly instancing from this pool
    auto key = conn->GetHash();
    auto originated_it = acquired_.find(key);
    if (originated_it == acquired_.end()) {
      // If its not originated from here then return false.
      // This connection doesnt belong to this connection pool,
      // How come it can be lost and find way to come to here?
      return false;
    }

    // Check-in the returned time
    auto node_ptr = originated_it->second;
    node_ptr->second->Returned();

    // Store back to queu
    connections_.push(node_ptr);

    // Remove from acquired list
    acquired_.erase(key);

    // signal waiting thread if conn available
    cv_main_.Signal();

    return true;
  }

  const bool& IsRun() const {
    return is_run_;
  }

 protected:
  std::string name_;
  StorageConfig& config_;
  ConnectionCreatePrimaryCallback create_primary_connection_callback_;
  ConnectionCreateStandbyCallback create_secondary_connection_callback_;

  // Store original connection
  // Never surrender the ownership to the leasor
  absl::node_hash_map<size_t, ConnectionPtr> connection_storages_;

  // Store the leased connection
  // Only store pointer to the real connection
  // inside connection_storages_
  absl::node_hash_map<size_t, std::pair<const size_t, ConnectionPtr>*>
      acquired_;

  // Queue for quick leasing the connections
  // Only store pointer to real connection
  // inside connection_storages_
  std::queue<std::pair<const size_t, ConnectionPtr>*> connections_;

  // Main mutex to handle the data
  mutable absl::Mutex mutex_main_;

  absl::CondVar cv_main_;

  bool is_run_;
  bool is_ready_;

  threads::EventLoopExecutor services_;

  void InitializePrimaryConnections() {
    auto min_conn = config_.PoolConfig().MinConnection() == 0
                        ? DEFAULT_WORKER_MINIMAL
                        : config_.PoolConfig().MinConnection();

    for (size_t i = 0; i < min_conn; i++) {
      auto conn = create_primary_connection_callback_(name_, &config_);
      auto key = conn->GetHash();

      // get pointer to the node
      auto back = connection_storages_.emplace(key, std::move(conn));
      if (!back.second)
        throw BadAllocationException("Bad alloc during initialize the "
                                     "connection into connection pool");

      auto it = back.first;
      std::pair<const size_t, ConnectionPtr>* node_ptr = &(*it);

      // emplace the pointer to the queue;
      connections_.push(node_ptr);

      // open the connection
      node_ptr->second->Open();
    }
  }

  void InitializeServices() {
    auto ping_interval =
        absl::Seconds(config_.PoolConfig().PingServerInterval().count() == 0
                          ? DEFAULT_IDLE_PING.count()
                          : config_.PoolConfig().PingServerInterval().count());

    auto cleanup_interval =
        absl::Seconds(config_.PoolConfig().CleanupInterval().count() == 0
                          ? DEFAULT_CLEANUP_INTERVAL.count()
                          : config_.PoolConfig().CleanupInterval().count());

    services_.SubmitTask([this]() { PingService(); },
                         threads::EventLoopExecutor::TaskType::RunAtInterval,
                         ping_interval, ping_interval);

    services_.SubmitTask([this]() { CleanupService(); },
                         threads::EventLoopExecutor::TaskType::RunAtInterval,
                         cleanup_interval, cleanup_interval);
  }

  void RunImpl() {
    {
      absl::MutexLockMaybe lock(&mutex_main_);
      // std::cout << "Initialize connections..." << std::endl;

      if (!create_primary_connection_callback_)
        throw InvalidArgException(
            "Null-reference on \"ConnectionCreateCallback callback\".");
      is_run_ = true;

      InitializePrimaryConnections();
      InitializeServices();
      is_ready_ = true;
      cv_main_.SignalAll();
    }
  }

  void PingService() {
    // Lock the main, we need to ensure no operations while we ping the
    // server We're only ping connection that are currently not leased

    absl::MutexLock lock(&mutex_main_);
    if (!is_run_)
      return;

    std::cout << "[" << absl::Now() << "] " << "Ping running..." << std::endl;

    for (size_t i = connections_.size(); i > 0; --i) {
      auto element = connections_.front();
      connections_.pop();
      element->second->PingServerAsync();
      connections_.push(std::move(element));
    }
    cv_main_.SignalAll();
  }

  void CleanupService() {
    // Lock the main, we need to ensure no operations while we cleanup the
    // We're only cleanup connection that are currently not leased

    absl::MutexLock lock(&mutex_main_);
    if (!is_run_)
      return;

    if (connections_.empty())
      return;

    std::cout << "[" << absl::Now() << "] " << "Cleanup running..."
              << std::endl;

    for (size_t i = connections_.size(); i > 0; --i) {
      auto element = connections_.front();
      connections_.pop();

      if (element->second->StandbyMode() == ConnectionStandbyMode::Standby &&
          element->second->IsIdle()) {
        element->second->Release();
        element->second.reset();
      } else {
        connections_.push(std::move(element));
      }
    }
  }
};

NVSERV_END_NAMESPACE
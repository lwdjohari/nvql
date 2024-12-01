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
#include "nvserv/storages/connection.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/storage_config.h"
#include "nvserv/threads/event_loop_executor.h"
// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

typedef ConnectionPtr (*ConnectionCreatePrimaryCallback)(
    const std::string& name, const StorageConfig* config);
typedef ConnectionPtr (*ConnectionCreateStandbyCallback)(
    const std::string& name, const StorageConfig* config);

class ConnectionPool {
 public:
  static constexpr std::chrono::seconds DEFAULT_CLEANUP_INTERVAL =
      std::chrono::seconds(160);
  static constexpr std::chrono::seconds DEFAULT_IDLE_PING =
      std::chrono::seconds(30);
  static constexpr std::chrono::seconds DEFAULT_MAX_WAITING_FOR_CONNECTION =
      std::chrono::seconds(5);

  static constexpr uint16_t DEFAULT_WORKER_MINIMAL = 1;
  static constexpr uint16_t DEFAULT_WORKER_MAXIMAL = 1;

  explicit ConnectionPool(const std::string& name, const StorageConfig& config);
  virtual ~ConnectionPool();

  const std::string& Name() const;

  const ConnectionPoolConfig& Config() const;

  void Run();

  void SetPrimaryConnectionCallback(ConnectionCreatePrimaryCallback callback);

  void RemovePrimaryConnectionCallback();

  void SetStandbyConnectionCallback(ConnectionCreateStandbyCallback callback);

  void RemoveStandbyConnectionCallback();

  void Stop();

  void StopImpl();

  ConnectionPtr Acquire();

  bool Return(ConnectionPtr conn);

  const bool& IsRun() const;

 protected:
  std::string name_;
  const StorageConfig& config_;
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

  void InitializePrimaryConnections();

  void InitializeServices();

  void RunImpl();

  void PingService();

  void CleanupService();
};

NVSERV_END_NAMESPACE
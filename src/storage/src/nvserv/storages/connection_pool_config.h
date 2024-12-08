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

#include <nvm/threads/task_pool.h>

#include <chrono>

#include "nvserv/declare.h"
#include "nvserv/global_macro.h"
#include "nvserv/storages/cluster_config.h"
#include "nvserv/storages/cluster_config_list.h"
#include "nvserv/storages/declare.h"

NVSERV_BEGIN_NAMESPACE(storages)
class ConnectionPoolConfig {
 public:
  explicit ConnectionPoolConfig(
      uint16_t min_connection, uint16_t max_connection, bool keep_alive = true,
      std::chrono::seconds connection_timeout = std::chrono::seconds(5),
      std::chrono::seconds ping_server_interval = std::chrono::seconds(30),
      std::chrono::seconds connection_idle_wait = std::chrono::seconds(120),
      std::chrono::seconds max_waiting_for_connection = std::chrono::seconds(5),
      std::chrono::seconds max_waiting_for_trans_creation =
          std::chrono::seconds(5),
      std::chrono::seconds cleanup_interval = std::chrono::seconds(45));

  const uint16_t& MinConnection() const;

  const uint16_t& MaxConnection() const ;

  const bool& KeepAlive() const ;

  const std::chrono::seconds& ConnectionTimeout() const;

  const std::chrono::seconds& PingServerInterval() const ;

  const std::chrono::seconds& ConnectionIdleWait() const ;

  const std::chrono::seconds& MaxWaitingForConnectionAvailable() const ;

  const std::chrono::seconds& MaxWaitingForTransactionCreation() const ;

  const std::chrono::seconds& CleanupInterval() const ;

 protected:
  uint16_t min_connection_;
  uint16_t max_connection_;
  bool keep_alive_;
  std::chrono::seconds connection_timeout_;
  std::chrono::seconds ping_server_interval_;
  std::chrono::seconds connection_idle_wait_;
  std::chrono::seconds max_waiting_for_connection_;
  std::chrono::seconds max_waiting_for_trans_creation_;
  std::chrono::seconds cleanup_interval_;
};

NVSERV_END_NAMESPACE
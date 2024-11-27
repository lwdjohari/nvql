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

#include "nvserv/storages/connection_pool_config.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

ConnectionPoolConfig::ConnectionPoolConfig(
    int16_t min_connection, int16_t max_connection, bool keep_alive,
    std::chrono::seconds connection_timeout,
    std::chrono::seconds ping_server_interval,
    std::chrono::seconds connection_idle_wait,
    std::chrono::seconds max_waiting_for_connection,
    std::chrono::seconds max_waiting_for_trans_creation,
    std::chrono::seconds cleanup_interval)
                : min_connection_(min_connection),
                  max_connection_(max_connection),
                  keep_alive_(keep_alive),
                  connection_timeout_(connection_timeout),
                  ping_server_interval_(ping_server_interval),
                  connection_idle_wait_(connection_idle_wait),
                  max_waiting_for_connection_(max_waiting_for_connection),
                  max_waiting_for_trans_creation_(
                      max_waiting_for_trans_creation),
                  cleanup_interval_(cleanup_interval) {}

const int16_t& ConnectionPoolConfig::MinConnection() const {
  return min_connection_;
}

const int16_t& ConnectionPoolConfig::MaxConnection() const {
  return min_connection_;
}

const bool& ConnectionPoolConfig::KeepAlive() const {
  return keep_alive_;
}

const std::chrono::seconds& ConnectionPoolConfig::ConnectionTimeout() const {
  return connection_timeout_;
}

const std::chrono::seconds& ConnectionPoolConfig::PingServerInterval() const {
  return ping_server_interval_;
}

const std::chrono::seconds& ConnectionPoolConfig::ConnectionIdleWait() const {
  return connection_idle_wait_;
}

const std::chrono::seconds&
ConnectionPoolConfig::MaxWaitingForConnectionAvailable() const {
  return max_waiting_for_connection_;
}

const std::chrono::seconds&
ConnectionPoolConfig::MaxWaitingForTransactionCreation() const {
  return max_waiting_for_trans_creation_;
}

const std::chrono::seconds& ConnectionPoolConfig::CleanupInterval() const {
  return cleanup_interval_;
}

NVSERV_END_NAMESPACE
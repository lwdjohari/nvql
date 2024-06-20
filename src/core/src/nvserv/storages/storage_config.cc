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

#include "nvserv/storages/storage_config.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

StorageConfigBase::StorageConfigBase() = default;

StorageConfig::StorageConfig(StorageType type, TransactionMode transact_mode,
                             bool pool_support,
                             ConnectionPoolConfig&& pool_config,
                             TaskPoolMode task_pool_mode,
                             nvm::threads::TaskPoolPtr task_pool,
                             ConnectionMode connection_mode,
                             ClusterConfigList&& cluster_configs)
                : StorageConfigBase(),
                  pool_config_(std::forward<ConnectionPoolConfig>(pool_config)),
                  task_pool_(task_pool),
                  cluster_configs_(
                      std::forward<ClusterConfigList>(cluster_configs)),
                  task_pool_mode_(task_pool_mode),
                  connection_mode_(connection_mode),
                  type_(type),
                  transact_mode_supported_(transact_mode),
                  pool_support_(pool_support),
                  async_support_(task_pool_mode != TaskPoolMode::None ? true
                                                                      : false) {
}

StorageConfig::~StorageConfig() = default;

const TaskPoolMode& StorageConfig::TaskPoolOption() const {
  return task_pool_mode_;
}

const bool& StorageConfig::ConnectionPoolingSupport() const {
  return pool_support_;
}

const bool& StorageConfig::AsyncExecutionSupport() const {
  return async_support_;
}

const TransactionMode& StorageConfig::TransactionModeSupported() const {
  return transact_mode_supported_;
}

const StorageType& StorageConfig::Type() const {
  return type_;
}

const ClusterConfigList& StorageConfig::ClusterConfigs() const {
  return cluster_configs_;
}

const ConnectionMode& StorageConfig::ConnectionModeOption() const {
  return connection_mode_;
}

const ConnectionPoolConfig& StorageConfig::PoolConfig() const {
  return pool_config_;
}

NVSERV_END_NAMESPACE
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
#include "nvserv/storages/connection_pool_config.h"
#include "nvserv/storages/declare.h"

// cppchekc-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

class StorageConfigBase {
 public:
  virtual const TaskPoolMode& TaskPoolOption() const = 0;
  virtual const bool& ConnectionPoolingSupport() const = 0;
  virtual const bool& AsyncExecutionSupport() const = 0;
  virtual const TransactionMode& TransactionModeSupported() const = 0;
  virtual const StorageType& Type() const = 0;
  virtual const ClusterConfigList& ClusterConfigs() const = 0;
  virtual const ConnectionMode& ConnectionModeOption() const = 0;
  virtual const ConnectionPoolConfig& PoolConfig() const = 0;

 protected:
  StorageConfigBase();
};

class StorageConfig : public StorageConfigBase {
 public:
  virtual ~StorageConfig();

  const TaskPoolMode& TaskPoolOption() const override;

  const bool& ConnectionPoolingSupport() const override;

  const bool& AsyncExecutionSupport() const override;

  const TransactionMode& TransactionModeSupported() const override;

  const StorageType& Type() const override;

  const ClusterConfigList& ClusterConfigs() const override;

  const ConnectionMode& ConnectionModeOption() const override;

  const ConnectionPoolConfig& PoolConfig() const override;

 protected:
  explicit StorageConfig(StorageType type, TransactionMode transact_mode,
                         bool pool_support, ConnectionPoolConfig&& pool_config,
                         TaskPoolMode task_pool_mode,
                         nvm::threads::TaskPoolPtr task_pool,
                         ConnectionMode connection_mode,
                         ClusterConfigList&& cluster_configs);

 private:
  ConnectionPoolConfig pool_config_;
  nvm::threads::TaskPoolPtr task_pool_;
  ClusterConfigList cluster_configs_;
  TaskPoolMode task_pool_mode_;
  ConnectionMode connection_mode_;
  StorageType type_;
  TransactionMode transact_mode_supported_;
  bool pool_support_;
  bool async_support_;
};

NVSERV_END_NAMESPACE
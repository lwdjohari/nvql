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

#include "nvserv/storages/connection_pool.h"
#include "nvserv/storages/postgres/declare.h"
#include "nvserv/storages/postgres/pg_cluster_config.h"
#include "nvserv/storages/postgres/pg_connection.h"
#include "nvserv/storages/postgres/pg_storage_config.h"
#include "nvserv/storages/postgres/pg_transaction.h"
#include "nvserv/storages/storage_config.h"
#include "nvserv/storages/storage_server.h"
#include "nvserv/storages/transaction.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgServer final : public StorageServer {
 public:
#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
  /// @brief Create component based PgServer. Do not use directly
  /// Call it from `RegisterStorage<TStorageComponent>(storage_id)`
  /// @param locator
  /// @param config
  explicit PgServer(const components::ComponentLocator& locator,
                    const components::ComponentConfig& config);

  /// @brief Create non-component based PgServer.
  /// Component Service Locator can not be used.
  /// You have manually manage the PgServer lifecycle and passing.
  /// @param name
  /// @param clusters
  /// @param pool_min_worker
  /// @param pool_max_worker
  explicit PgServer(const std::string& name,
                    std::initializer_list<PgClusterConfig> clusters,
                    uint16_t pool_min_worker = 5,
                    u_int16_t pool_max_worker = 10);

#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  explicit PgServer(const std::string& name,
                    std::initializer_list<PgClusterConfig> clusters,
                    uint16_t pool_min_worker = 5,
                    u_int16_t pool_max_worker = 10);

#endif

  virtual ~PgServer();

  const std::string& Name() const override;

  bool TryConnect() override;

  bool Shutdown(
      bool grace_shutdown = true,
      std::chrono::seconds deadline = std::chrono::seconds(0)) override;

  TransactionPtr Begin(
      TransactionMode mode = TransactionMode::ReadWrite) override;

  const StorageConfig& Configs() const override;

  const PgStorageConfig& PgConfigs() const;

  const ConnectionPoolPtr Pool() const override;

  ConnectionPoolPtr Pool() override;

  StorageInfo GetStorageServerInfo() const override;

  static PgServerPtr MakePgServer(
      const std::string& name, std::initializer_list<PgClusterConfig> clusters,
      uint16_t pool_min_worker = 5, u_int16_t pool_max_worker = 10);

 private:
  std::string name_;

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
  std::shared_ptr<PgStorageConfig> configs_storage_;
  const PgStorageConfig& configs_;
#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  PgStorageConfig configs_;
#endif

  ConnectionPoolPtr pools_;

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  PgStorageConfig CreateConfig(const std::vector<PgClusterConfig>& clusters,
                               uint16_t pool_min_worker,
                               u_int16_t pool_max_worker);
#endif

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
  std::shared_ptr<PgStorageConfig> CreateConfig(
      const std::vector<PgClusterConfig>& clusters, uint16_t pool_min_worker,
      u_int16_t pool_max_worker);
#endif

  ConnectionPoolPtr CreatePools();

  static ConnectionPtr CreatePrimaryPgConnection(const std::string& name,
                                                 const StorageConfig* config);

  static ConnectionPtr CreateStandbyPgConnection(const std::string& name,
                                                 const StorageConfig* config);
};

NVSERV_END_NAMESPACE
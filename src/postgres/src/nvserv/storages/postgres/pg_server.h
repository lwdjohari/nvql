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
                    const components::ComponentConfig& config)
                  : StorageServer(locator, config,
                                  components::ComponentType::kPostgresFeature),
                    configs_(
                        static_cast<const postgres::PgStorageConfig&>(config)),
                    pools_(CreatePools()) {}

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
                    u_int16_t pool_max_worker = 10)
                  : StorageServer(components::ComponentType::kPostgresFeature),
                    name_(std::string(name)),
                    configs_storage_(CreateConfig(clusters, pool_min_worker,
                                                  pool_max_worker)),
                    configs_(*configs_storage_),
                    pools_(CreatePools()) {}
#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  explicit PgServer(const std::string& name,
                    std::initializer_list<PgClusterConfig> clusters,
                    uint16_t pool_min_worker = 5,
                    u_int16_t pool_max_worker = 10)
                  : StorageServer(),
                    name_(std::string(name)),
                    configs_(CreateConfig(clusters, pool_min_worker,
                                          pool_max_worker)),
                    pools_(CreatePools()) {}
#endif

  ~PgServer() {}

  const std::string& Name() const override {
    return name_;
  }

  bool TryConnect() override {
    pools_->Run();
    return true;
  }

  bool Shutdown(
      bool grace_shutdown = true,
      std::chrono::seconds deadline = std::chrono::seconds(0)) override {
    pools_->Stop();
    return false;
  }

  TransactionPtr Begin(
      TransactionMode mode = TransactionMode::ReadWrite) override {
    return __NR_RETURN_MOVE(std::make_shared<PgTransaction>(this, mode));
  }

  const StorageConfig& Configs() const override {
    return configs_;
  }

  const PgStorageConfig& PgConfigs() const {
    return configs_;
  }

  const ConnectionPoolPtr Pool() const override {
    return pools_;
  }

  ConnectionPoolPtr Pool() override {
    return pools_;
  }

  StorageInfo GetStorageServerInfo() const override {
    return StorageInfo();
  }

  static PgServerPtr MakePgServer(
      const std::string& name, std::initializer_list<PgClusterConfig> clusters,
      uint16_t pool_min_worker = 5, u_int16_t pool_max_worker = 10) {
    return __NR_RETURN_MOVE(std::make_shared<postgres::PgServer>(
        name, clusters, pool_min_worker, pool_max_worker));
  }

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
                               u_int16_t pool_max_worker) {
    if (clusters.size() == 0)
      throw StorageException("No cluster configs, please define at least one "
                             "connection to postgres DB Server",
                             StorageType::Postgres);

    ClusterConfigList cluster_configs(StorageType::Postgres);
    for (const auto& cluster : clusters) {
      cluster_configs.Add(std::move(PgClusterConfig(cluster)));
    }

    ConnectionPoolConfig pool_config(pool_min_worker, pool_max_worker);

    PgStorageConfig config(std::move(cluster_configs), std::move(pool_config));

    return __NR_RETURN_MOVE(config);
  }
#endif

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
  std::shared_ptr<PgStorageConfig> CreateConfig(
      const std::vector<PgClusterConfig>& clusters, uint16_t pool_min_worker,
      u_int16_t pool_max_worker) {
    if (clusters.size() == 0)
      throw StorageException("No cluster configs, please define at least one "
                             "connection to postgres DB Server",
                             StorageType::Postgres);

    ClusterConfigList cluster_configs(StorageType::Postgres);
    for (const auto& cluster : clusters) {
      cluster_configs.Add(std::move(PgClusterConfig(cluster)));
    }

    ConnectionPoolConfig pool_config(pool_min_worker, pool_max_worker);

    return __NR_RETURN_MOVE(std::make_shared<PgStorageConfig>(
        std::move(cluster_configs), std::move(pool_config)));
  }
#endif

  ConnectionPoolPtr CreatePools() {
#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
    auto pools = std::make_shared<ConnectionPool>(name_, configs_);
#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
    auto pools = std::make_shared<ConnectionPool>(name_, configs_);
#endif

    pools_->SetPrimaryConnectionCallback(PgServer::CreatePrimaryPgConnection);
    pools_->SetStandbyConnectionCallback(PgServer::CreateStandbyPgConnection);

    return __NR_RETURN_MOVE(pools);
  }

  static ConnectionPtr CreatePrimaryPgConnection(const std::string& name,
                                                 const StorageConfig* config) {
    if (!config)
      throw Exception("Config is null");

    auto conn = std::make_shared<PgConnection>(
        name, config->ClusterConfigs(), ConnectionStandbyMode::Primary,
        config->PoolConfig().ConnectionIdleWait());

    return __NR_RETURN_MOVE(conn);
  }

  static ConnectionPtr CreateStandbyPgConnection(const std::string& name,
                                                 const StorageConfig* config) {
    auto conn = std::make_shared<PgConnection>(
        name, config->ClusterConfigs(), ConnectionStandbyMode::Standby,
        config->PoolConfig().ConnectionIdleWait());

    return __NR_RETURN_MOVE(conn);
  }
};

// Late declare

std::shared_ptr<PgConnection> PgTransaction::GetConnectionFromPool() {
  if (!server_)
    throw storages::TransactionException(
        "PgServer is Null, Unable to get connection from pool",
        StorageType::Postgres);

  auto conn = server_->Pool()->Acquire();
  if (!conn)
    throw storages::TransactionException(
        "Transaction Begin failed, can't acquired connection from pool",
        StorageType::Postgres);
  auto res = std::static_pointer_cast<PgConnection>(conn);
  return __NR_RETURN_MOVE(res);
}

void PgTransaction::ReturnConnectionToThePool() {
  if (server_ != nullptr && connection_ != nullptr) {
    server_->Pool()->Return(connection_);
  }

  server_ = nullptr;
}

NVSERV_END_NAMESPACE
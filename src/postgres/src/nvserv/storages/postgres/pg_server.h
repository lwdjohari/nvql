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
  explicit PgServer(const components::ComponentLocator& locator,
                    const components::ComponentConfig& config)
                  : StorageServer(locator, config,
                                  components::ComponentType::kPostgresFeature),
                    configs_(CreateConfig(clusters, pool_min_worker,
                                          pool_max_worker)),
                    pools_(CreatePools()) {}
#endif


#if  defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  explicit PgServer(const std::string& name,
                    std::initializer_list<PgClusterConfig> clusters,
                    uint16_t pool_min_worker = 5,
                    u_int16_t pool_max_worker = 10)
                  : StorageServer(),
                    name_(std::string(name)),
                    configs_(CreateConfig(clusters, pool_min_worker,
                                          pool_max_worker)),
                    pools_(CreatePools()) {}

  // explicit PgServer(const PgStorageConfig& config)
  //                 : StorageServer(), configs_(config) {}

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

/**
 *  If nvql as part of nvserv lib, we must passing it to Configurator Service 
 *  NVQL_STANDALONE=0
 */

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
  static PgServerPtr MakePgServer(
      const std::string& name, std::initializer_list<PgClusterConfig> clusters,
      uint16_t pool_min_worker = 5, u_int16_t pool_max_worker = 10);
#endif

/**
 *  If as nvql lib passing the configuration directly
 *  NVQL_STANDALONE=0
 */

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  static PgServerPtr MakePgServer(
      const std::string& name, std::initializer_list<PgClusterConfig> clusters,
      uint16_t pool_min_worker = 5, u_int16_t pool_max_worker = 10) {
    return __NR_RETURN_MOVE(std::make_shared<postgres::PgServer>(
        name, clusters, pool_min_worker, pool_max_worker));
  }
#endif

 private:
  std::string name_;
  PgStorageConfig configs_;
  ConnectionPoolPtr pools_;

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

  ConnectionPoolPtr CreatePools() {
    auto pools = std::make_shared<ConnectionPool>(name_, configs_);
    pools_->SetPrimaryConnectionCallback(PgServer::CreatePrimaryPgConnection);
    pools_->SetStandbyConnectionCallback(PgServer::CreateStandbyPgConnection);

    return __NR_RETURN_MOVE(pools);
  }

  static ConnectionPtr CreatePrimaryPgConnection(const std::string& name,
                                                 StorageConfig* config) {
    if (!config)
      throw Exception("Config is null");

    auto conn = std::make_shared<PgConnection>(
        name, config->ClusterConfigs(), ConnectionStandbyMode::Primary,
        config->PoolConfig().ConnectionIdleWait());

    return __NR_RETURN_MOVE(conn);
  }

  static ConnectionPtr CreateStandbyPgConnection(const std::string& name,
                                                 StorageConfig* config) {
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
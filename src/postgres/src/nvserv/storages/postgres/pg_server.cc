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

#include "nvserv/storages/postgres/pg_server.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0

PgServer::PgServer(const components::ComponentLocator& locator,
                   const components::ComponentConfig& config)
                : StorageServer(locator, config,
                                components::ComponentType::kPostgresFeature),
                  configs_(
                      static_cast<const postgres::PgStorageConfig&>(config)),
                  pools_(CreatePools()) {};

PgServer::PgServer(const std::string& name,
                   std::initializer_list<PgClusterConfig> clusters,
                   uint16_t pool_min_worker, u_int16_t pool_max_worker)
                : StorageServer(components::ComponentType::kPostgresFeature),
                  name_(std::string(name)),
                  configs_storage_(
                      CreateConfig(clusters, pool_min_worker, pool_max_worker)),
                  configs_(*configs_storage_),
                  pools_(CreatePools()) {}
#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
PgServer::PgServer(const std::string& name,
                   std::initializer_list<PgClusterConfig> clusters,
                   uint16_t pool_min_worker, u_int16_t pool_max_worker)
                : StorageServer(),
                  name_(std::string(name)),
                  configs_(
                      CreateConfig(clusters, pool_min_worker, pool_max_worker)),
                  pools_(CreatePools()) {}
#endif

PgServer::~PgServer() {}

const std::string& PgServer::Name() const {
  return name_;
}

bool PgServer::TryConnect() {
  pools_->Run();
  return true;
}

bool PgServer::Shutdown(bool grace_shutdown, std::chrono::seconds deadline) {
  pools_->Stop();
  return false;
}

TransactionPtr PgServer::Begin(TransactionMode mode) {
  return __NR_RETURN_MOVE(std::make_shared<PgTransaction>(this, mode));
}

const StorageConfig& PgServer::Configs() const {
  return configs_;
}

const PgStorageConfig& PgServer::PgConfigs() const {
  return configs_;
}

 ConnectionPoolPtr PgServer::Pool() const {
  return pools_;
}

ConnectionPoolPtr PgServer::Pool() {
  return pools_;
}

StorageInfo PgServer::GetStorageServerInfo() const {
  return StorageInfo();
}

// static
PgServerPtr PgServer::MakePgServer(
    const std::string& name, std::initializer_list<PgClusterConfig> clusters,
    uint16_t pool_min_worker, u_int16_t pool_max_worker) {
  return __NR_RETURN_MOVE(std::make_shared<postgres::PgServer>(
      name, clusters, pool_min_worker, pool_max_worker));
}

// private:

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
PgStorageConfig PgServer::CreateConfig(
    const std::vector<PgClusterConfig>& clusters, uint16_t pool_min_worker,
    u_int16_t pool_max_worker) {
  if (clusters.empty())
    throw StorageException("No cluster configs, please define at least one "
                           "connection to postgres DB Server",
                           StorageType::Postgres);

  ClusterConfigList cluster_configs(StorageType::Postgres);
  for (const auto& cluster : clusters) {
    cluster_configs.Add(std::move(PgClusterConfig(cluster)));
  }

  PgStorageConfig config(
    std::move(cluster_configs), 
    ConnectionPoolConfig(pool_min_worker, pool_max_worker));

  return __NR_RETURN_MOVE(config);
}
#endif

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
std::shared_ptr<PgStorageConfig> PgServer::CreateConfig(
    const std::vector<PgClusterConfig>& clusters, uint16_t pool_min_worker,
    u_int16_t pool_max_worker) {
  if (clusters.empty()) {
    throw StorageException("No cluster configs, please define at least one "
                           "connection to postgres DB Server",
                           StorageType::Postgres);
  }

  ClusterConfigList cluster_configs(StorageType::Postgres);

  for (const auto& cluster : clusters) {
    cluster_configs.Add(std::move(PgClusterConfig(cluster)));
  }

  return __NR_RETURN_MOVE(std::make_shared<PgStorageConfig>(
      std::move(cluster_configs),
      ConnectionPoolConfig(pool_min_worker, pool_max_worker)));
}
#endif

ConnectionPoolPtr PgServer::CreatePools() {
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

// static
ConnectionPtr PgServer::CreatePrimaryPgConnection(const std::string& name,
                                                  const StorageConfig* config) {
  if (!config) {
    throw Exception("Config is null");
  }

  auto conn = std::make_shared<PgConnection>(
      name, config->ClusterConfigs(), ConnectionStandbyMode::Primary,
      config->PoolConfig().ConnectionIdleWait());

  return __NR_RETURN_MOVE(conn);
}

// static
ConnectionPtr PgServer::CreateStandbyPgConnection(const std::string& name,
                                                  const StorageConfig* config) {
  auto conn = std::make_shared<PgConnection>(
      name, config->ClusterConfigs(), ConnectionStandbyMode::Standby,
      config->PoolConfig().ConnectionIdleWait());

  return __NR_RETURN_MOVE(conn);
}

// Late declare

std::shared_ptr<PgConnection> PgTransaction::GetConnectionFromPool() {
  if (!server_) {
    throw storages::TransactionException(
        "PgServer is Null, Unable to get connection from pool",
        StorageType::Postgres);
  }
  auto conn = server_->Pool()->Acquire();
  if (!conn) {
    throw storages::TransactionException(
        "Transaction Begin failed, can't acquired connection from pool",
        StorageType::Postgres);
  }
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
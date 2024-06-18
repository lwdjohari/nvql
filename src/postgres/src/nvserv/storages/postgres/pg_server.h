#pragma once

#include "nvserv/storages/config.h"
#include "nvserv/storages/connection_pool.h"
#include "nvserv/storages/postgres/declare.h"
#include "nvserv/storages/postgres/pg_cluster_config.h"
#include "nvserv/storages/postgres/pg_connection.h"
#include "nvserv/storages/postgres/pg_storage_config.h"
#include "nvserv/storages/postgres/pg_transaction.h"
#include "nvserv/storages/storage_server.h"
#include "nvserv/storages/transaction.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgServer final : public StorageServer {
 public:
#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 0
  explicit PgServer(const components::ComponentLocator& locator,
                    const components::ComponentConfig& config)
                  : StorageServer(locator, config,
                                  components::ComponentType::kPostgresFeature),
                    configs_(storages::StorageType::Postgres) {}
#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1

  explicit PgServer(std::initializer_list<PgClusterConfig> clusters,
                    uint16_t pool_min_worker = 5,
                    u_int16_t pool_max_worker = 10)
                  : StorageServer(),
                    configs_(CreateConfig(clusters, pool_min_worker,
                                          pool_max_worker)),
                    pools_(CreatePools()) {}

  // explicit PgServer(const PgStorageConfig& config)
  //                 : StorageServer(), configs_(config) {}
#endif

  ~PgServer() {}

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

 private:
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
    auto pools = std::make_shared<ConnectionPool>(configs_);
    pools_->SetPrimaryConnectionCallback(PgServer::CreatePrimaryPgConnection);
    pools_->SetStandbyConnectionCallback(PgServer::CreateStandbyPgConnection);

    return __NR_RETURN_MOVE(pools);
  }

  static ConnectionPtr CreatePrimaryPgConnection(StorageConfig* config) {
    if(!config)
      throw Exception("Config is null");

    auto conn = std::make_shared<PgConnection>(
        config->ClusterConfigs(),
        ConnectionStandbyMode::Primary,
        config->PoolConfig().ConnectionIdleWait());

    return __NR_RETURN_MOVE(conn);
  }

  static ConnectionPtr CreateStandbyPgConnection(StorageConfig* config) {
    auto conn = std::make_shared<PgConnection>(
        config->ClusterConfigs(),
        ConnectionStandbyMode::Standby,
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
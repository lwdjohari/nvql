#pragma once

#include "nvserv/storages/config.h"
#include "nvserv/storages/postgres/declare.h"
#include "nvserv/storages/postgres/pg_cluster_config.h"
#include "nvserv/storages/postgres/pg_connection.h"
#include "nvserv/storages/postgres/pg_storage_config.h"
#include "nvserv/storages/postgres/pg_transaction.h"
#include "nvserv/storages/storage_server.h"
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
                                          pool_max_worker)) {}

  explicit PgServer(const PgStorageConfig& config)
                  : StorageServer(), configs_(config) {}
#endif

  ~PgServer() {}

  bool TryConnect() override {
    return false;
  }

  bool Shutdown(
      bool grace_shutdown = true,
      std::chrono::seconds deadline = std::chrono::seconds(0)) override {
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
    return nullptr;
  }

  ConnectionPoolPtr Pool() override {
    return nullptr;
  }

  StorageInfo GetStorageServerInfo() const override {
    return StorageInfo();
  }

 private:
  PgStorageConfig configs_;

  PgStorageConfig CreateConfig(const std::vector<PgClusterConfig>& clusters,
                               uint16_t pool_min_worker,
                               u_int16_t pool_max_worker) {
    return configs_;
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

  return __NR_RETURN_MOVE(std::static_pointer_cast<PgConnection>(conn));
}

void PgTransaction::ReturnConnectionToThePool() {
  if (server_ != nullptr && connection_ != nullptr) {
    server_->Pool()->Return(connection_);
  }

  server_ = nullptr;
}

NVSERV_END_NAMESPACE
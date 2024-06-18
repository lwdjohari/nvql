#pragma once

#include <nvm/threads/task_pool.h>

#include <chrono>

#include "nvserv/declare.h"
#include "nvserv/global_macro.h"
#include "nvserv/storages/config.h"
#include "nvserv/storages/postgres/pg_cluster_config.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgStorageConfig : public StorageConfig {
 public:
  explicit PgStorageConfig(ClusterConfigList&& cluster_config,
                           ConnectionPoolConfig&& pool_config)
                  : 
                    StorageConfig(StorageType::Postgres,
                                  TransactionMode::ReadCommitted |
                                      TransactionMode::ReadOnly |
                                      TransactionMode::ReadWrite,
                                  true, std::forward<ConnectionPoolConfig>(pool_config), TaskPoolMode::Internal,
                                  nullptr, ConnectionMode::ServerCluster,
                                  std::forward<ClusterConfigList>(cluster_config)) {}

};

NVSERV_END_NAMESPACE
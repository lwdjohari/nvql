#pragma once

#include <nvm/threads/task_pool.h>

#include <chrono>

#include "nvserv/declare.h"
#include "nvserv/global_macro.h"
#include "nvserv/storages/cluster_config.h"
#include "nvserv/storages/declare.h"
// cppchekc-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)
class ConnectionPoolConfig {
 public:
  explicit ConnectionPoolConfig(
      int16_t min_connection, int16_t max_connection, bool keep_alive = true,
      std::chrono::seconds connection_timeout = std::chrono::seconds(5),
      std::chrono::seconds ping_server_interval = std::chrono::seconds(120),
      std::chrono::seconds connection_idle_wait = std::chrono::seconds(300),
      std::chrono::seconds max_waiting_for_connection =
          std::chrono::seconds(10),
      std::chrono::seconds max_waiting_for_trans_creation =
          std::chrono::seconds(10),
      std::chrono::seconds cleanup_interval = std::chrono::seconds(120))
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

  const int16_t& MinConnection() const {
    return min_connection_;
  }

  const int16_t& MaxConnection() const {
    return min_connection_;
  }

  const bool& KeepAlive() const {
    return keep_alive_;
  }

  const std::chrono::seconds& ConnectionTimeout() const {
    return connection_timeout_;
  }

  const std::chrono::seconds& PingServerInterval() const {
    return ping_server_interval_;
  }

  const std::chrono::seconds& ConnectionIdleWait() const {
    return connection_idle_wait_;
  }

  const std::chrono::seconds& MaxWaitingForConnectionAvailable() const {
    return max_waiting_for_connection_;
  }

  const std::chrono::seconds& MaxWaitingForTransactionCreation() const {
    return max_waiting_for_trans_creation_;
  }

  const std::chrono::seconds& CleanupInterval() const {
    return cleanup_interval_;
  }

 protected:
  int16_t min_connection_;
  int16_t max_connection_;
  bool keep_alive_;
  std::chrono::seconds connection_timeout_;
  std::chrono::seconds ping_server_interval_;
  std::chrono::seconds connection_idle_wait_;
  std::chrono::seconds max_waiting_for_connection_;
  std::chrono::seconds max_waiting_for_trans_creation_;
  std::chrono::seconds cleanup_interval_;
};

class ConfigBase {
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
  ConfigBase() = default;
};

class StorageConfig : public ConfigBase {
 public:
  virtual ~StorageConfig() = default;

  const TaskPoolMode& TaskPoolOption() const override {
    return task_pool_mode_;
  }

  const bool& ConnectionPoolingSupport() const override {
    return pool_support_;
  }

  const bool& AsyncExecutionSupport() const override {
    return async_support_;
  }

  const TransactionMode& TransactionModeSupported() const override {
    return transact_mode_supported_;
  }

  const StorageType& Type() const override {
    return type_;
  }

  const ClusterConfigList& ClusterConfigs() const override {
    return cluster_configs_;
  }

  const ConnectionMode& ConnectionModeOption() const override {
    return connection_mode_;
  }

  const ConnectionPoolConfig& PoolConfig() const override {
    return pool_config_;
  }

 protected:
  explicit StorageConfig(StorageType type, TransactionMode transact_mode,
                  bool pool_support, ConnectionPoolConfig&& pool_config,
                  TaskPoolMode task_pool_mode,
                  nvm::threads::TaskPoolPtr task_pool,
                  ConnectionMode connection_mode,
                  ClusterConfigList&& cluster_configs)
                  : ConfigBase(),
                    pool_config_(
                        std::forward<ConnectionPoolConfig>(pool_config)),
                    task_pool_(task_pool),
                    cluster_configs_(
                        std::forward<ClusterConfigList>(cluster_configs)),
                    task_pool_mode_(task_pool_mode),
                    connection_mode_(connection_mode),
                    type_(type),
                    transact_mode_supported_(transact_mode),
                    pool_support_(pool_support),
                    async_support_(
                        task_pool_mode != TaskPoolMode::None ? true : false) {}

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
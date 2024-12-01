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

#include <exception>
#include <memory>
#include <pqxx/pqxx>

#include "nvm/random.h"
#include "nvserv/global_macro.h"
#include "nvserv/storages/connection.h"
#include "nvserv/storages/exceptions.h"
#include "nvserv/storages/postgres/pg_cluster_config.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgConnection : public Connection {
 public:
  explicit PgConnection(
      const std::string& name, const ClusterConfigList& clusters,
      ConnectionStandbyMode standby_mode,
      std::chrono::seconds mark_idle_after = std::chrono::seconds(300));
                  
  ~PgConnection();

  TransactionPtr CreateTransaction() override;

  void Open() override;

  void Close() override;

  bool IsOpen() const override;

  ///< Ping server to report keep-alive.
  /// Different storage server has different implementations
  /// to guarantee the connection is keep-alive
  void PingServerAsync() override;

  virtual bool PingServer() override;

  TransactionMode SupportedTransactionMode() const override;

  void ReportHealth() const override;

  ConnectionMode Mode() const override;

  void Release() override;

  size_t GetHash() const override;

  const std::string& GetConnectionString() const override;

  std::optional<std::pair<std::string, bool>> PrepareStatement(
      __NR_STRING_COMPAT_REF query) override;

  pqxx::connection* Driver();

 protected:
  void OpenImpl() override;

  void CloseImpl() override;

  void ReleaseImpl() override;

 private:
  ClusterConfigList clusters_;
  std::string connection_string_;
  std::unique_ptr<pqxx::connection> conn_;
  ConnectionMode mode_;
  std::hash<std::string> hash_fn_;
  size_t hash_key_;

  size_t CreateHashKey();

  std::string BuildConnectionString();
};

NVSERV_END_NAMESPACE
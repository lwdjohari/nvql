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
#include "nvserv/storages/postgres/pg_connection.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

PgConnection::PgConnection(const std::string& name,
                           const ClusterConfigList& clusters,
                           ConnectionStandbyMode standby_mode,
                           std::chrono::seconds mark_idle_after)
                : Connection(name, StorageType::Postgres, standby_mode,
                             mark_idle_after),
                  clusters_(clusters),
                  connection_string_(BuildConnectionString()),
                  conn_(nullptr),
                  mode_(),
                  hash_key_(CreateHashKey()) {}

PgConnection::~PgConnection() {}

TransactionPtr PgConnection::CreateTransaction() {
  return nullptr;
}

void PgConnection::Open() {
  OpenImpl();
}

void PgConnection::Close() {
  CloseImpl();
}

bool PgConnection::IsOpen() const {
  if (!conn_) {
    return false;
  }

  return conn_->is_open();
}

///< Ping server to report keep-alive.
/// Different storage server has different implementations
/// to guarantee the connection is keep-alive
void PgConnection::PingServerAsync() {
  std::cout << "Ping Server: " << GetHash() << "\n";
}

bool PgConnection::PingServer() {
  std::cout << "Ping Server" << "\n";
  return true;
}

TransactionMode PgConnection::SupportedTransactionMode() const {
  return TransactionMode::ReadCommitted | TransactionMode::ReadOnly |
         TransactionMode::ReadWrite;
}

void PgConnection::ReportHealth() const {
  std::cout << "Health Report" << "\n";
}

ConnectionMode PgConnection::Mode() const {
  return mode_;
}

void PgConnection::Release() {
  ReleaseImpl();
}

size_t PgConnection::GetHash() const {
  return hash_key_;
}

const std::string& PgConnection::GetConnectionString() const {
  return connection_string_;
};

std::optional<std::pair<std::string, bool>> PgConnection::PrepareStatement(
    __NR_STRING_COMPAT_REF query) {
  if (!prepared_statement_manager_) {
    throw NullReferenceException("Null reference to PrepareStatemenManagerPtr "
                                 "in prepared_statement_manager_");
  }

  return std::move(prepared_statement_manager_->Register(query));
}

pqxx::connection* PgConnection::Driver() {
  return conn_.get();
}

// protected

void PgConnection::OpenImpl() {
  if (conn_) {
    throw ConnectionException("Connection already created",
                              StorageType::Postgres);
  }

  try {
    // std::cout << "Connection string:" << connection_string_ << std::endl;

    conn_ = std::make_unique<pqxx::connection>(connection_string_);

  } catch (const pqxx::broken_connection& e) {
    throw ConnectionException(e.what(), StorageType::Postgres);
  } catch (const pqxx::in_doubt_error& e) {
    throw InternalErrorException(e.what(), StorageType::Postgres);
  } catch (const pqxx::usage_error& e) {
    throw InternalErrorException(e.what(), StorageType::Postgres);
  } catch (const pqxx::internal_error& e) {
    throw InternalErrorException(e.what(), StorageType::Postgres);
  } catch (const std::exception& e) {
    throw ConnectionException(e.what(), StorageType::Postgres);
  } catch (...) {
    throw StorageException("Unknown exception caught.", StorageType::Postgres);
  }
}

void PgConnection::CloseImpl() {
  if (!conn_) {
    return;
  }
  if (!conn_->is_open()) {
    return;
  }

  try {
    // Explicitly close the connection
    conn_->close();
    std::cout << "Connection to database closed successfully." << "\n";
  } catch (const pqxx::broken_connection& e) {
    throw ConnectionException(e.what(), StorageType::Postgres);
  } catch (const std::exception& e) {
    throw ConnectionException(e.what(), StorageType::Postgres);
  } catch (...) {
    throw StorageException("Unknown exception caught during connection close.",
                           StorageType::Postgres);
  }
}

void PgConnection::ReleaseImpl() {
  if (!conn_) {
    return;
  }

  Close();
  conn_ = nullptr;
}

// private

size_t PgConnection::CreateHashKey() {
  std::ostringstream ss;
  ss << "{" << ToStringEnumStorageType(type_) << " | "
     << std::to_string(nvm::utils::RandomizeUint32t()) << "}";

  auto feed = ss.str();
  auto hash_key = hash_fn_(feed);

  return hash_key;
}

std::string PgConnection::BuildConnectionString() {
  if (clusters_.Configs().empty()) {
    return std::string();
  }

  std::ostringstream host;
  std::string dbname;

  uint16_t index = 0;

  for (const auto& cluster : clusters_.Configs()) {
    auto pg_cluster = std::dynamic_pointer_cast<PgClusterConfig>(cluster);
    if (index == 0) {
      host << "postgresql://" << std::string(pg_cluster->User()) << ":"
           << std::string(pg_cluster->Password()) << "@";
      dbname = std::string(pg_cluster->DbName());

      host << pg_cluster->Host() << ":" << pg_cluster->Port();
      index++;
      continue;
    }

    host << "," << pg_cluster->Host() << ":" << pg_cluster->Port();

    index++;
  }

  host << "/" << dbname << "?application_name=" << name_ << "::nvql"
       << "&connect_timeout=5";

  return __NR_RETURN_MOVE(host.str());
}

NVSERV_END_NAMESPACE
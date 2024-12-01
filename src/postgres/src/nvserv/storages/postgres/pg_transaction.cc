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

#include "nvserv/storages/postgres/pg_transaction.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

namespace impl {

// PgInnerTransactionBase

PgInnerTransactionBase::~PgInnerTransactionBase() {
  conn_ = nullptr;
}

ExecutionResultPtr PgInnerTransactionBase::Execute(
    const __NR_STRING_COMPAT_REF query_key,
    const parameters::ParameterArgs& args) {
  throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                      StorageType::Postgres);
}

ExecutionResultPtr PgInnerTransactionBase::ExecuteNonPrepared(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                      StorageType::Postgres);
}

void PgInnerTransactionBase::Commit() {
  throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                      StorageType::Postgres);
}
void PgInnerTransactionBase::Rollback() {
  throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                      StorageType::Postgres);
}

PgInnerTransactionType PgInnerTransactionBase::Type() {
  return type_;
}

// protected:
PgInnerTransactionBase::PgInnerTransactionBase(pqxx::connection* conn,
                                               PgInnerTransactionType type)
                : conn_(conn), type_(type) {};

/** PgWorkTransaction */

PgWorkTransaction::PgWorkTransaction(pqxx::connection* conn)
                : PgInnerTransactionBase(conn,
                                         PgInnerTransactionType::ReadWrite),
                  txn_(CreateTransaction()) {}

ExecutionResultPtr PgWorkTransaction::Execute(
    const __NR_STRING_COMPAT_REF query_key,
    const parameters::ParameterArgs& args) {
  if (args.size() == 0) {
    auto result = txn_.exec_prepared(__NR_CALL_STRING_COMPAT_REF(query_key));
    return std::make_shared<PgExecutionResult>(std::move(result));
  } else {
    pqxx::params params;
    TranslateParams(params, args);
    auto result =
        txn_.exec_prepared(__NR_CALL_STRING_COMPAT_REF(query_key), params);
    return std::make_shared<PgExecutionResult>(std::move(result));
  }
}

ExecutionResultPtr PgWorkTransaction::ExecuteNonPrepared(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  if (args.size() == 0) {
    auto result = txn_.exec(__NR_CALL_STRING_COMPAT_REF(query));
    return std::make_shared<PgExecutionResult>(std::move(result));
  } else {
    pqxx::params params;
    TranslateParams(params, args);
    auto result = txn_.exec_params(__NR_CALL_STRING_COMPAT_REF(query), params);
    return std::make_shared<PgExecutionResult>(std::move(result));
  }
}

void PgWorkTransaction::Commit() {
  txn_.commit();
}

void PgWorkTransaction::Rollback() {
  txn_.abort();
}

pqxx::work PgWorkTransaction::CreateTransaction() {
  return pqxx::work(*conn_);
}

/* PgNonTransaction */

PgNonTransaction::PgNonTransaction(pqxx::connection* conn)
                : PgInnerTransactionBase(
                      conn, PgInnerTransactionType::NonTransaction),
                  txn_(CreateTransaction()) {};

ExecutionResultPtr PgNonTransaction::Execute(
    const __NR_STRING_COMPAT_REF query_key,
    const parameters::ParameterArgs& args) {
  if (args.size() == 0) {
    auto result = txn_.exec_prepared(__NR_CALL_STRING_COMPAT_REF(query_key));
    return std::make_shared<PgExecutionResult>(std::move(result));
  } else {
    pqxx::params params;
    TranslateParams(params, args);
    auto result =
        txn_.exec_prepared(__NR_CALL_STRING_COMPAT_REF(query_key), params);
    return std::make_shared<PgExecutionResult>(std::move(result));
  }
}

ExecutionResultPtr PgNonTransaction::ExecuteNonPrepared(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  if (args.size() == 0) {
    auto result = txn_.exec(__NR_CALL_STRING_COMPAT_REF(query));
    return std::make_shared<PgExecutionResult>(std::move(result));
  } else {
    pqxx::params params;
    TranslateParams(params, args);
    auto result = txn_.exec_params(__NR_CALL_STRING_COMPAT_REF(query), params);
    return std::make_shared<PgExecutionResult>(std::move(result));
  }
}

void PgNonTransaction::Commit() {
  // No commit necessary for non-transaction
}

void PgNonTransaction::Rollback() {
  // No rollback necessary for non-transaction
}

pqxx::nontransaction PgNonTransaction::CreateTransaction() {
  return pqxx::nontransaction(*conn_);
}

/* PgReadOnlyTransaction */

PgReadOnlyTransaction::PgReadOnlyTransaction(pqxx::connection* conn)
                : PgInnerTransactionBase(conn,
                                         PgInnerTransactionType::ReadOnly),
                  txn_(CreateTransaction()) {};

ExecutionResultPtr PgReadOnlyTransaction::Execute(
    const __NR_STRING_COMPAT_REF query_key,
    const parameters::ParameterArgs& args) {
  if (args.size() == 0) {
    auto result = txn_.exec_prepared(__NR_CALL_STRING_COMPAT_REF(query_key));
    return std::make_shared<PgExecutionResult>(std::move(result));
  } else {
    pqxx::params params;
    TranslateParams(params, args);
    auto result =
        txn_.exec_prepared(__NR_CALL_STRING_COMPAT_REF(query_key), params);
    return std::make_shared<PgExecutionResult>(std::move(result));
  }
}

ExecutionResultPtr PgReadOnlyTransaction::ExecuteNonPrepared(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  if (args.size() == 0) {
    auto result = txn_.exec(__NR_CALL_STRING_COMPAT_REF(query));
    return std::make_shared<PgExecutionResult>(std::move(result));
  } else {
    pqxx::params params;
    TranslateParams(params, args);
    auto result = txn_.exec_params(__NR_CALL_STRING_COMPAT_REF(query), params);
    return std::make_shared<PgExecutionResult>(std::move(result));
  }
}

void PgReadOnlyTransaction::Commit() {
  txn_.commit();
}

void PgReadOnlyTransaction::Rollback() {
  txn_.abort();
}

pqxx::read_transaction PgReadOnlyTransaction::CreateTransaction() {
  return pqxx::read_transaction(*conn_);
}

}  // namespace impl

/* PgSubTransaction is Template so it on .h
*/

// Alias for the variant type used in parameters, covering common PostgreSQL
// types
// using PgInternalParamType =
//     std::variant<int,          // Integer type
//                  double,       // Floating-point type
//                  std::string,  // String type, including JSON and JSONB
//                  bool,         // Boolean type
//                  std::chrono::system_clock::time_point,  // Timestamp type
//                  std::vector<unsigned char>,             // Binary data type
//                  std::array<unsigned char, 16>,          // UUID type
//                  pqxx::binarystring,                     // Binary string
//                  type std::int16_t,                           // Small
//                  integer type std::int64_t,                           // Big
//                  integer type float,                                  // Real
//                  (float) type std::tm,                                // Date
//                  type pqxx::zview                             // Text array
//                  type
//                  >;

/* PgTransaction */

PgTransaction::PgTransaction(PgServer* server, TransactionMode mode)
                : Transaction(StorageType::Postgres, mode),
                  server_(server),
                  connection_(GetConnectionFromPool()),
                  transact_(CreateTransaction()) {}

PgTransaction::~PgTransaction() {
  // must be returned the borrowed connection from connection pool
  ReturnConnectionToThePool();
}

void PgTransaction::Commit() {
  try {
    transact_->Commit();
  } catch (const std::exception& e) {
    // Handle commit failure
    throw std::runtime_error(std::string("Commit failed: ") + e.what());
  }
}

void PgTransaction::Rollback() {
  try {
    transact_->Rollback();
  } catch (const std::exception& e) {
    // Handle rollback failure
    throw std::runtime_error(std::string("Rollback failed: ") + e.what());
  }
}

// protected:

ExecutionResultPtr PgTransaction::ExecuteImpl(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  auto key = connection_->PrepareStatement(query);
  if (!key.has_value())
    throw TransactionException("Exceptions on empty sql query on Execute",
                               StorageType::Postgres);

  if (key.value().second)
    connection_->Driver()->prepare(key.value().first,
                                   __NR_CALL_STRING_COMPAT_REF(query));

  return __NR_RETURN_MOVE(transact_->Execute(key.value().first, args));
}

ExecutionResultPtr PgTransaction::ExecuteNonPreparedImpl(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  if (query.empty())
    throw TransactionException("Exceptions on empty sql query on Execute",
                               StorageType::Postgres);

  return __NR_RETURN_MOVE(transact_->ExecuteNonPrepared(query, args));
}

// private:

std::unique_ptr<impl::PgInnerTransactionBase>
PgTransaction::CreateTransaction() {
  switch (mode_) {
    case TransactionMode::ReadWrite:
      return std::make_unique<impl::PgWorkTransaction>(connection_->Driver());
    case TransactionMode::ReadOnly:
    case TransactionMode::ReadCommitted:
      return std::make_unique<impl::PgReadOnlyTransaction>(
          connection_->Driver());
    case TransactionMode::NonTransaction:
      return std::make_unique<impl::PgNonTransaction>(connection_->Driver());
    default:
      throw storages::TransactionException(
          "Postgres unsupported Transaction Mode: " +
              ToStringEnumTransactionMode(mode_),
          StorageType::Postgres);
  }
}

NVSERV_END_NAMESPACE
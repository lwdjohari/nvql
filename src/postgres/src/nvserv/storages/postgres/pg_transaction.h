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

#include <iostream>
#include <pqxx/pqxx>
#include <variant>

#include "nvserv/global_macro.h"
#include "nvserv/storages/connection_pool.h"
#include "nvserv/storages/postgres/declare.h"
#include "nvserv/storages/postgres/pg_column.h"
#include "nvserv/storages/postgres/pg_connection.h"
#include "nvserv/storages/postgres/pg_execution_result.h"
#include "nvserv/storages/transaction.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

namespace impl {
enum class PgInnerTransactionType {
  Unknown,
  ReadWrite,
  ReadOnly,
  NonTransaction,
  SubTransaction
};

inline void TranslateParams(pqxx::params& params,
                            const parameters::ParameterArgs& nvql_params) {
  using namespace parameters;
  for (const auto& param : nvql_params) {
    switch (param.Type()) {
      case DataType::SmallInt:
        params.append(param.As<int16_t>());
        break;
      case DataType::Int:
        params.append(param.As<int32_t>());
        break;
      case DataType::BigInt:
        params.append(param.As<int64_t>());
        break;
      case DataType::Double:
        params.append(param.As<double>());
        break;
      case DataType::Real:
        params.append(param.As<float>());
        break;
      case DataType::String:
        params.append(param.As<std::string>());
        break;
      case DataType::Boolean:
        params.append(param.As<bool>());
        break;
      // case DataType::Timestamp:
      //   params.append(param.As<std::chrono::system_clock::time_point>());
      //   break;
      case DataType::Timestampz:
        params.append(param.As<NvDateTime>().ToIso8601());
        break;
      // case DataType::Date:
      //   params.append(param.As<std::vector<unsigned char>>());
      //   break;
      default:
        throw std::invalid_argument("Unsupported data type");
    }
  }
};

class PgInnerTransactionBase {
 public:
  virtual ~PgInnerTransactionBase();

  virtual ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query_key,
                                     const parameters::ParameterArgs& args);

  virtual ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args);

  virtual void Commit();
  virtual void Rollback();

  PgInnerTransactionType Type();

 protected:
  PgInnerTransactionBase(pqxx::connection* conn, PgInnerTransactionType type);

  pqxx::connection* conn_;
  PgInnerTransactionType type_;
};

/**
 * @class PgWorkTransaction
 * @brief PostgreSQL read-write transaction class.
 */
class PgWorkTransaction final : public PgInnerTransactionBase {
 public:
  /**
   * @brief Constructor.
   * @param conn The PostgreSQL connection.
   */
  explicit PgWorkTransaction(pqxx::connection* conn);

  /**
   * @brief Execute a prepared statement.
   * @param query_key The query key.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query_key,
                             const parameters::ParameterArgs& args) override;

  /**
   * @brief Execute a non-prepared statement.
   * @param query The query.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override;

  /**
   * @brief Commit the transaction.
   */
  void Commit() override;
  /**
   * @brief Rollback the transaction.
   */
  void Rollback() override;

 private:
  pqxx::work txn_;  ///< The PostgreSQL work transaction.

  /**
   * @brief Create a new work transaction.
   * @return The created work transaction.
   */
  pqxx::work CreateTransaction();
};

/**
 * @class PgNonTransaction
 * @brief PostgreSQL non-transactional execution class.
 */
class PgNonTransaction final : public PgInnerTransactionBase {
 public:
  /**
   * @brief Constructor.
   * @param conn The PostgreSQL connection.
   */
  explicit PgNonTransaction(pqxx::connection* conn);

  /**
   * @brief Execute a prepared statement.
   * @param query_key The query key.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query_key,
                             const parameters::ParameterArgs& args) override;

  /**
   * @brief Execute a non-prepared statement.
   * @param query The query.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override;

  /**
   * @brief Commit the transaction. No operation for non-transaction.
   */
  void Commit() override;

  /**
   * @brief Rollback the transaction. No operation for non-transaction.
   */
  void Rollback() override;

 private:
  pqxx::nontransaction txn_;  ///< The PostgreSQL non-transaction.

  /**
   * @brief Create a new non-transaction.
   * @return The created non-transaction.
   */
  pqxx::nontransaction CreateTransaction();
};

/**
 * @class PgReadOnlyTransaction
 * @brief PostgreSQL read-only transaction class.
 */
class PgReadOnlyTransaction final : public PgInnerTransactionBase {
 public:
  /**
   * @brief Constructor.
   * @param conn The PostgreSQL connection.
   */
  explicit PgReadOnlyTransaction(pqxx::connection* conn);

  /**
   * @brief Execute a prepared statement.
   * @param query_key The query key.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query_key,
                             const parameters::ParameterArgs& args) override;

  /**
   * @brief Execute a non-prepared statement.
   * @param query The query.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override;

  /**
   * @brief Commit the transaction.
   */
  void Commit() override;

  /**
   * @brief Rollback the transaction.
   */
  void Rollback() override;

 private:
  pqxx::read_transaction txn_;  ///< The PostgreSQL read-only transaction.

  /**
   * @brief Create a new read-only transaction.
   * @return The created read-only transaction.
   */
  pqxx::read_transaction CreateTransaction();
};

/**
 * @class PgSubTransaction
 * @brief PostgreSQL subtransaction class.
 */
template <typename TParentTrans = pqxx::work>
class PgSubTransaction final : public PgInnerTransactionBase {
 public:
  /**
   * @brief Constructor.
   * @param parent_txn The parent transaction.
   */
  explicit PgSubTransaction(TParentTrans& parent_txn)
                  : PgInnerTransactionBase(
                        nullptr, PgInnerTransactionType::SubTransaction),
                    txn_(parent_txn) {}

  /**
   * @brief Execute a prepared statement.
   * @param query_key The query key.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query_key,
                             const parameters::ParameterArgs& args) override {
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

  /**
   * @brief Execute a non-prepared statement.
   * @param query The query.
   * @param args The parameters for the query.
   * @return The execution result.
   */
  ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override {
    if (args.size() == 0) {
      auto result = txn_.exec(__NR_CALL_STRING_COMPAT_REF(query));
      return std::make_shared<PgExecutionResult>(std::move(result));
    } else {
      pqxx::params params;
      TranslateParams(params, args);
      auto result =
          txn_.exec_params(__NR_CALL_STRING_COMPAT_REF(query), params);
      return std::make_shared<PgExecutionResult>(std::move(result));
    }
  }

  /**
   * @brief Commit the subtransaction.
   */
  void Commit() override {
    txn_.commit();
  }

  /**
   * @brief Rollback the subtransaction.
   */
  void Rollback() override {
    txn_.abort();
  }

 private:
  pqxx::subtransaction txn_;  ///< The PostgreSQL subtransaction.
};

}  // namespace impl

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

class PgTransaction : public Transaction {
 public:
  explicit PgTransaction(PgServer* server, TransactionMode mode);

  virtual ~PgTransaction();

  void Commit() override;

  void Rollback() override;

 protected:
  ExecutionResultPtr ExecuteImpl(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override;

  ExecutionResultPtr ExecuteNonPreparedImpl(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override;

 private:
  PgServer* server_;
  std::shared_ptr<PgConnection> connection_;
  std::unique_ptr<impl::PgInnerTransactionBase> transact_;

  std::shared_ptr<PgConnection> GetConnectionFromPool();
  void ReturnConnectionToThePool();

  std::unique_ptr<impl::PgInnerTransactionBase> CreateTransaction();
};

NVSERV_END_NAMESPACE
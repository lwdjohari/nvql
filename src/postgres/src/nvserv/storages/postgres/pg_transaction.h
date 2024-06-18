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
enum class PgInnerTransactionType { Unknown, ReadWrite, ReadOnly };

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
  virtual ~PgInnerTransactionBase() {
    conn_ = nullptr;
  }

  virtual ExecutionResultPtr Execute(const std::string& query_key,
                                     const parameters::ParameterArgs& args) {
    throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                        StorageType::Postgres);
  }
  virtual void Commit() {
    throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                        StorageType::Postgres);
  }
  virtual void Rollback() {
    throw nvserv::storages::UnsupportedFeatureException("Unimplemented Execute",
                                                        StorageType::Postgres);
  }

  PgInnerTransactionType Type() {
    return type_;
  }

 protected:
  PgInnerTransactionBase(pqxx::connection* conn, PgInnerTransactionType type)
                  : conn_(conn), type_(type) {}
  pqxx::connection* conn_;
  PgInnerTransactionType type_;
};

class PgWorkTransaction final : public PgInnerTransactionBase {
 public:
  explicit PgWorkTransaction(pqxx::connection* conn)
                  : PgInnerTransactionBase(conn,
                                           PgInnerTransactionType::ReadWrite),
                    txn_(CreateTransaction()) {}

  ExecutionResultPtr Execute(const std::string& query_key,
                             const parameters::ParameterArgs& args) override {
    if (args.size() == 0) {
      auto result = txn_.exec_prepared(query_key);
      return std::make_shared<PgExecutionResult>(result);
    } else {
      pqxx::params params;
      TranslateParams(params, args);
      auto result = txn_.exec_prepared(query_key, params);
      return std::make_shared<PgExecutionResult>(result);
    }
  }
  virtual void Commit() override {
    txn_.commit();
  }
  virtual void Rollback() override {
    txn_.abort();
  }

 private:
  pqxx::work txn_;

  pqxx::work CreateTransaction() {
    return pqxx::work(*conn_);
  }
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
  explicit PgTransaction(PgServer* server, TransactionMode mode)
                  : Transaction(StorageType::Postgres, mode),
                    server_(server),
                    connection_(GetConnectionFromPool()),
                    transact_(CreateTransaction()) {}
  virtual ~PgTransaction() {
    // must be returned the borrowed connection from connection pool
    ReturnConnectionToThePool();
  }

  void Commit() override {
    try {
      transact_.Commit();
    } catch (const std::exception& e) {
      // Handle commit failure
      throw std::runtime_error(std::string("Commit failed: ") + e.what());
    }
  }

  void Rollback() override {
    try {
      transact_.Rollback();
    } catch (const std::exception& e) {
      // Handle rollback failure
      throw std::runtime_error(std::string("Rollback failed: ") + e.what());
    }
  }

 protected:
  // ExecutionResultPtr ExecuteImpl(const __NR_STRING_COMPAT_REF query,
  //                                const std::vector<std::any>& args) override {
  //   // conn_->prepare("qname", std::string(query.data()));
  //   //  auto stmnt = txn_.prepared("qname");

  //   return nullptr;
  // }

  ExecutionResultPtr ExecuteImpl(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) override {
    auto key = connection_->PrepareStatement(query);

    if (!key.has_value())
      throw TransactionException("Exceptions on empty sql query on Execute",
                                 StorageType::Postgres);
    return __NR_RETURN_MOVE(transact_.Execute(key.value(), args));
  }

 private:
  PgServer* server_;
  std::shared_ptr<PgConnection> connection_;
  impl::PgInnerTransactionBase transact_;

  std::shared_ptr<PgConnection> GetConnectionFromPool();
  void ReturnConnectionToThePool();

  impl::PgInnerTransactionBase CreateTransaction() {
    switch (mode_) {
      case TransactionMode::ReadWrite:
        return impl::PgWorkTransaction(connection_->Driver());
      case TransactionMode::ReadOnly:
      case TransactionMode::ReadCommitted:
        return impl::PgWorkTransaction(connection_->Driver());
      default:
        throw storages::UnsupportedFeatureException(
            "Postgres unsupported Transaction Mode: " +
                ToStringEnumTransactionMode(mode_),
            StorageType::Postgres);
    }
  }
};

NVSERV_END_NAMESPACE
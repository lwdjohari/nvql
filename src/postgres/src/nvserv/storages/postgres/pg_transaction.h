#pragma once

#include <iostream>
#include <pqxx/pqxx>
#include <variant>

#include "nvserv/global_macro.h"
#include "nvserv/storages/postgres/declare.h"
#include "nvserv/storages/postgres/pg_column.h"
#include "nvserv/storages/postgres/pg_execution_result.h"
#include "nvserv/storages/transaction.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

// Alias for the variant type used in parameters, covering common PostgreSQL
// types
using PgInternalParamType =
    std::variant<int,          // Integer type
                 double,       // Floating-point type
                 std::string,  // String type, including JSON and JSONB
                 bool,         // Boolean type
                 std::chrono::system_clock::time_point,  // Timestamp type
                 std::vector<unsigned char>,             // Binary data type
                 std::array<unsigned char, 16>,          // UUID type
                 pqxx::binarystring,                     // Binary string type
                 std::int16_t,                           // Small integer type
                 std::int64_t,                           // Big integer type
                 float,                                  // Real (float) type
                 std::tm,                                // Date type
                 pqxx::zview                             // Text array type
                 >;

class PgTransaction : public Transaction {
 public:
  explicit PgTransaction(PgServer* server)
                  : Transaction(),
                    server_(server),
                    conn_(nullptr),
                    txn_(CreateTransaction()) {}
  virtual ~PgTransaction() {
    // must be returned the borrowed connection from connection pool
  }

  void Commit() override {
    try {
      txn_.commit();
    } catch (const std::exception& e) {
      // Handle commit failure
      throw std::runtime_error(std::string("Commit failed: ") + e.what());
    }
  }

  void Rollback() override {
    try {
      txn_.abort();
    } catch (const std::exception& e) {
      // Handle rollback failure
      throw std::runtime_error(std::string("Rollback failed: ") + e.what());
    }
  }

 protected:
  ExecutionResultPtr ExecuteImpl(const __NR_STRING_COMPAT_REF query,
                                 const std::vector<std::any>& args) override {
    conn_->prepare("qname", std::string(query.data()));
    // auto stmnt = txn_.prepared("qname");

    return nullptr;
  }

 private:
  PgServer* server_;
  pqxx::connection* conn_;
  pqxx::work txn_;

  pqxx::work CreateTransaction() {
    return pqxx::work(*conn_);
  }
};

NVSERV_END_NAMESPACE
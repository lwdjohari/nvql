#pragma once
#include <pqxx/pqxx>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/execution_result.h"
#include "nvserv/storages/postgres/pg_row_result.h"
#include "nvserv/storages/postgres/pg_row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgExecutionResult : public ExecutionResult {
  explicit PgExecutionResult(pqxx::result result)
                  : ExecutionResult(StorageType::Postgres),
                    result_(std::move(result)) {}

  bool Empty() const override {
    return result_.empty();
  }

  size_t RowAffected() const override {
    return result_.affected_rows();
  }

  RowResultPtr At(const size_t& offset) const override {
    if (offset >= result_.size()) {
      throw std::out_of_range("Offset out of range");
    }
    return std::make_shared<PgRowResult>(result_[offset]);
  }

  RowResultIterator begin() const override {
    return PgRowResultIterator(result_, 0);
  }

  RowResultIterator end() const override {
    return PgRowResultIterator(result_, result_.size());
  }

 private:
  pqxx::result result_;
};

NVSERV_END_NAMESPACE
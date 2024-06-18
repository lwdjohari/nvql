#pragma once

#include <iterator>
#include <memory>
#include <pqxx/pqxx>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/postgres/pg_row_result.h"
#include "nvserv/storages/row_result.h"
#include "nvserv/storages/row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgRowResultIterator : public RowResultIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = RowResultPtr;
  using difference_type = std::ptrdiff_t;
  using pointer = const RowResultPtr*;
  using reference = const RowResultPtr&;

  explicit PgRowResultIterator(const pqxx::result& result, size_t index)
      : result_(result), index_(index) {}

  PgRowResultIterator& operator++() override {
    ++index_;
    return *this;
  }

  bool operator==(const RowResultIterator& other) const override {
    auto other_pg = dynamic_cast<const PgRowResultIterator*>(&other);
    return other_pg && &result_ == &(other_pg->result_) && index_ == other_pg->index_;
  }

  bool operator!=(const RowResultIterator& other) const override {
    return !(*this == other);
  }

  RowResultPtr operator*() const override {
    return std::make_shared<PgRowResult>(std::make_shared<pqxx::row>(result_.at(index_)));
  }

  std::unique_ptr<RowResultIterator> clone() const override {
    return std::make_unique<PgRowResultIterator>(*this);
  }

 private:
  const pqxx::result& result_;
  size_t index_;
};

NVSERV_END_NAMESPACE
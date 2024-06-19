#pragma once

#include <any>
#include <tuple>
#include <utility>
#include <memory>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/row_result.h"
#include "nvserv/storages/row_result_iterator.h"
#include "nvserv/storages/mapper.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ExecutionResult {
 public:
  virtual ~ExecutionResult() = default;
  virtual bool Empty() const = 0;
  virtual size_t RowAffected() const = 0;
  virtual RowResultPtr At(const size_t& offset) const = 0;
  virtual std::unique_ptr<RowResultIterator> begin() const = 0;
  virtual std::unique_ptr<RowResultIterator> end() const = 0;
  StorageType Type() const {
    return type_;
  }

 protected:
  explicit ExecutionResult(StorageType type) : type_(type) {}

 private:
  StorageType type_;
};


#pragma once


class Cursor {
 public:
  explicit Cursor(const ExecutionResult& exec_result)
      : exec_result_(exec_result) {}

  class Iterator {
   public:
    explicit Iterator(std::unique_ptr<RowResultIterator> iter) : iter_(std::move(iter)) {}

    Iterator& operator++() {
      ++(*iter_);
      return *this;
    }

    bool operator!=(const Iterator& other) const {
      return *iter_ != *(other.iter_);
    }

    RowResultPtr operator*() const {
      return **iter_;
    }

   private:
    std::unique_ptr<RowResultIterator> iter_;
  };

  Iterator begin() const {
    return Iterator(exec_result_.begin());
  }

  Iterator end() const {
    return Iterator(exec_result_.end());
  }

 private:
  const ExecutionResult& exec_result_;
};


NVSERV_END_NAMESPACE
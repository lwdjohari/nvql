#pragma once

#include <any>
#include <tuple>
#include <utility>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/row_result.h"
#include "nvserv/storages/row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ExecutionResult {
 public:
  virtual ~ExecutionResult() {}
  virtual bool Empty() const = 0;
  virtual size_t RowAffected() const = 0;
  virtual RowResultPtr At(const size_t& offset) const = 0;
  virtual RowResultIterator begin() const = 0;
  virtual RowResultIterator end() const = 0;
  StorageType Type() const {
    return type_;
  }

 protected:
  explicit ExecutionResult(StorageType type) : type_(type) {}

 private:
  StorageType type_;
};

NVSERV_END_NAMESPACE
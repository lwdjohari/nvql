#pragma once

#include <iterator>
#include <memory>
#include <pqxx/pqxx>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"



NVSERV_BEGIN_NAMESPACE(storages)

// Forward declaration
// class RowResult;
// using RowResultPtr = std::shared_ptr<RowResult>;


class RowResultIterator {
 public:
  virtual ~RowResultIterator() = default;
  virtual RowResultIterator& operator++() = 0;
  virtual bool operator==(const RowResultIterator& other) const = 0;
  virtual bool operator!=(const RowResultIterator& other) const = 0;
  virtual RowResultPtr operator*() const = 0;
  virtual std::unique_ptr<RowResultIterator> clone() const = 0;
};

NVSERV_END_NAMESPACE

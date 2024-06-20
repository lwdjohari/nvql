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
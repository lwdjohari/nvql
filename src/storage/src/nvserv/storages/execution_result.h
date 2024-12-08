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

#include <memory>
#include <tuple>
#include <utility>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/mapper.h"
#include "nvserv/storages/row_result.h"
#include "nvserv/storages/row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ExecutionResult {
 public:
  virtual ~ExecutionResult();
  virtual bool Empty() const = 0;
  virtual size_t RowAffected() const = 0;
  virtual RowResultPtr At(const int& offset) const = 0;
  virtual std::unique_ptr<RowResultIterator> begin() const = 0;
  virtual std::unique_ptr<RowResultIterator> end() const = 0;
  StorageType Type() const;

 protected:
  explicit ExecutionResult(StorageType type);

 private:
  StorageType type_;
};

class Cursor {
 public:
  explicit Cursor(const ExecutionResult& exec_result);

  class Iterator {
   public:
    explicit Iterator(std::unique_ptr<RowResultIterator> iter);

    Iterator& operator++();

    bool operator!=(const Iterator& other) const;

    RowResultPtr operator*() const;

   private:
    std::unique_ptr<RowResultIterator> iter_;
  };

  Iterator begin() const;

  Iterator end() const;

 private:
  const ExecutionResult& exec_result_;
};

NVSERV_END_NAMESPACE
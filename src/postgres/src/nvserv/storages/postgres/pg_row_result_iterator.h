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

  explicit PgRowResultIterator(const pqxx::result& result, size_t index);
                
  PgRowResultIterator& operator++() override;

  bool operator==(const RowResultIterator& other) const override;

  bool operator!=(const RowResultIterator& other) const override;

  RowResultPtr operator*() const override;

  std::unique_ptr<RowResultIterator> clone() const override;

 private:
  const pqxx::result& result_;
  size_t index_;
};

NVSERV_END_NAMESPACE
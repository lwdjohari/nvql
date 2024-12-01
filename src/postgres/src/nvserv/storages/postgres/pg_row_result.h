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

#include <chrono>
#include <memory>
#include <optional>
#include <pqxx/pqxx>
#include <stdexcept>
#include <vector>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/postgres/pg_column.h"
#include "nvserv/storages/postgres/pg_helper.h"
#include "nvserv/storages/row_result.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgRowResult : public RowResult {
 public:
  explicit PgRowResult(std::shared_ptr<pqxx::row> row);
  virtual ~PgRowResult();
  std::optional<Column> GetColumn(const std::string& columnName) const override;

  std::optional<Column> GetColumn(const size_t& index) const override;

  size_t Size() const override;

  // ColumnIterator begin() const {
  //     return ColumnIterator(row_, 0);
  // }

  // ColumnIterator end() const {
  //     return ColumnIterator(row_, row_.size());
  // }

  int16_t AsImpl_int16_t(const size_t& index) const override;

  int32_t AsImpl_int32_t(const size_t& index) const override;

  int64_t AsImpl_int64_t(const size_t& index) const override;

  std::string AsImpl_string(const size_t& index) const override;

  float AsImpl_float(const size_t& index) const override;

  double AsImpl_double(const size_t& index) const override;

  nvm::dates::DateTime AsImpl_DateTime_Timestampz(
      const size_t& index) const override;

  nvm::dates::DateTime AsImpl_DateTime_Timestamp(
      const size_t& index) const override;

  int16_t AsImpl_int16_t(const std::string& column_name) const override;

  int32_t AsImpl_int32_t(const std::string& column_name) const override;

  int64_t AsImpl_int64_t(const std::string& column_name) const override;

  std::string AsImpl_string(const std::string& column_name) const override;

  float AsImpl_float(const std::string& column_name) const override;

  double AsImpl_double(const std::string& column_name) const override;

  nvm::dates::DateTime AsImpl_DateTime_Timestampz(
      const std::string& column_name) const override;

  nvm::dates::DateTime AsImpl_DateTime_Timestamp(
      const std::string& column_name) const override;

 private:
  std::shared_ptr<pqxx::row> row_;
};

NVSERV_END_NAMESPACE

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
#include <optional>
#include <pqxx/pqxx>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "nvm/dates/datetime.h"
#include "nvm/macro.h"
#include "nvserv/global_macro.h"
#include "nvserv/storages/column.h"
#include "nvserv/storages/declare.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

// Define the template to check if T is the same as TType
template <typename T, typename TType>
constexpr bool is_type_v = std::is_same_v<T, TType>;

class RowResult {
 public:
  virtual std::optional<Column> GetColumn(
      const std::string& column_name) const {
    return std::nullopt;
  }

  virtual std::optional<Column> GetColumn(const size_t& index) const {
    return std::nullopt;
  }

  template <typename T>
  T As(const size_t& index) const {
    if constexpr (is_type_v<T, int64_t>) {
      return AsImpl_int64_t(index);
    } else if constexpr (is_type_v<T, int32_t>) {
      return AsImpl_int32_t(index);
    } else if constexpr (is_type_v<T, int16_t>) {
      return AsImpl_int16_t(index);
    } else if constexpr (is_type_v<T, float>) {
      return AsImpl_float(index);
    } else if constexpr (is_type_v<T, double>) {
      return AsImpl_double(index);
    } else if constexpr (is_type_v<T, std::string>) {
      return AsImpl_string(index);
    } else {
      static_assert("RowResult.As<T>, T is not supported by NvQL");
    }

    throw std::invalid_argument("RowResult.As<T>, T is not supported by NvQL");
  }

  template <typename T>
  T As(const std::string& column_name) const {
    if constexpr (is_type_v<T, int64_t>) {
      return AsImpl_int64_t(column_name);
    } else if constexpr (is_type_v<T, int32_t>) {
      return AsImpl_int32_t(column_name);
    } else if constexpr (is_type_v<T, int16_t>) {
      return AsImpl_int16_t(column_name);
    } else if constexpr (is_type_v<T, float>) {
      return AsImpl_float(column_name);
    } else if constexpr (is_type_v<T, double>) {
      return AsImpl_double(column_name);
    } else if constexpr (is_type_v<T, std::string>) {
      return AsImpl_string(column_name);
    } else {
      static_assert(is_type_v<T, int64_t>,
                    "RowResult.As<T>, T is not supported by NvQL");
    }

    throw std::invalid_argument("RowResult.As<T>, T is not supported by NvQL");
  }

  template <typename T>
  T AsDateTimeOffset(const size_t& index) {
    if constexpr (is_type_v<T, nvm::dates::DateTime>) {
      return AsImpl_DateTime_Timestampz(index);
    } else {
      static_assert(
          is_type_v<T, int64_t>,
          "RowResult.AsDateTimeOffset<T>, T is not supported by NvQL");
    }
  }

  template <typename T>
  T AsDateTimeOffset(const std::string& column_name) {
    if constexpr (is_type_v<T, nvm::dates::DateTime>) {
      return AsImpl_DateTime_Timestampz(column_name);
    } else {
      static_assert(
          is_type_v<T, int64_t>,
          "RowResult.AsDateTimeOffset<T>, T is not supported by NvQL");
    }
  }

  template <typename T>
  T AsDateTime(const size_t& index) {
    if constexpr (is_type_v<T, nvm::dates::DateTime>) {
      return AsImpl_DateTime_Timestamp(index);
    } else {
      static_assert(is_type_v<T, int64_t>,
                    "RowResult.AsDateTime<T>, T is not supported by NvQL");
    }
  }

  template <typename T>
  T AsDateTime(const std::string& column_name) {
    if constexpr (is_type_v<T, nvm::dates::DateTime>) {
      return AsImpl_DateTime_Timestamp(column_name);
    } else {
      static_assert(is_type_v<T, int64_t>,
                    "RowResult.AsDateTime<T>, T is not supported by NvQL");
    }
  }

  virtual size_t Size() const {
    throw std::runtime_error("Implement RowResult::Size() on derrived class");
  }

  // ColumnIterator begin() const {
  //     return ColumnIterator(row_, 0);
  // }

  // ColumnIterator end() const {
  //     return ColumnIterator(row_, row_.size());
  // }

 protected:
  explicit RowResult() {}

  virtual int16_t AsImpl_int16_t(const std::string& column_name) const = 0;
  virtual int16_t AsImpl_int16_t(const size_t& index) const = 0;
  virtual int32_t AsImpl_int32_t(const std::string& column_name) const = 0;
  virtual int32_t AsImpl_int32_t(const size_t& index) const = 0;
  virtual int64_t AsImpl_int64_t(const std::string& column_name) const = 0;
  virtual int64_t AsImpl_int64_t(const size_t& index) const = 0;
  virtual std::string AsImpl_string(const std::string& column_name) const = 0;
  virtual std::string AsImpl_string(const size_t& index) const = 0;
  virtual float AsImpl_float(const std::string& column_name) const = 0;
  virtual float AsImpl_float(const size_t& index) const = 0;
  virtual double AsImpl_double(const std::string& column_name) const = 0;
  virtual double AsImpl_double(const size_t& index) const = 0;
  virtual nvm::dates::DateTime AsImpl_DateTime_Timestampz(
      const std::string& column_name) const = 0;
  virtual nvm::dates::DateTime AsImpl_DateTime_Timestampz(
      const size_t& index) const = 0;
  virtual nvm::dates::DateTime AsImpl_DateTime_Timestamp(
      const std::string& column_name) const = 0;
  virtual nvm::dates::DateTime AsImpl_DateTime_Timestamp(
      const size_t& index) const = 0;
};

NVSERV_END_NAMESPACE

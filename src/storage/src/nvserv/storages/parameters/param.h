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
#include <utility>
#include <variant>

#include "nvserv/global_macro.h"

NVSERV_BEGIN_NAMESPACE(storages)

namespace parameters {

using NvQLVType = std::variant<
    std::reference_wrapper<const int16_t>,      // Small Int
    std::reference_wrapper<const int32_t>,      // Integer type
    std::reference_wrapper<const int64_t>,      // Bigint
    std::reference_wrapper<const double>,       // Floating-point type
    std::reference_wrapper<const float>,        // Real (float) type
    std::reference_wrapper<const std::string>,  // String type, including JSON
                                                // and JSONB
    std::reference_wrapper<const bool>,         // Boolean type
    std::reference_wrapper<
        const std::chrono::system_clock::time_point>,  // Timestamp type
    std::reference_wrapper<const NvDateTime>,          // Timestamp offset
    std::reference_wrapper<const std::vector<unsigned char>>,    // Binary data
                                                                 // type
    std::reference_wrapper<const std::array<unsigned char, 16>>  // UUID type
    >;

enum class DataType {
  SmallInt,
  Int,
  BigInt,
  Real,
  Double,
  String,
  Boolean,
  Date,
  Time,
  Timestamp,
  Timestampz
};

class Param {
 public:
  explicit Param(const int16_t& value);
  explicit Param(const int32_t& value);
  explicit Param(const int64_t& value);
  explicit Param(const double& value);
  explicit Param(const float& value);
  explicit Param(const std::string& value);
  explicit Param(const bool& value);
  explicit Param(const std::chrono::system_clock::time_point& value);
  explicit Param(const NvDateTime& value);

  static Param SmallInt(const int16_t& value);
  static Param Int(const int32_t& value);
  static Param BigInt(const int64_t& value);
  static Param Double(const double& value);
  static Param Real(const float& value);
  static Param String(const std::string& value);
  static Param Bool(const bool& value);
  static Param Timestamp(const std::chrono::system_clock::time_point& value);
  static Param Timestampz(const NvDateTime& value);

  // explicit Param(const char* value)
  //                 : data_(std::cref(std::string(value))),
  //                   type_(DataType::String) {}

  // explicit Param(const std::vector<unsigned char>& value)
  //                 : data_(std::cref(value)), type_(DataType::Date) {}
  // explicit Param(const std::array<unsigned char, 16>& value)
  //                 : data_(std::cref(value)), type_(DataType::Date) {}

  DataType Type() const;
  template <typename T>
  const T& As() const;

 private:
  NvQLVType data_;
  DataType type_;
};

template <typename T>
const T& Param::As() const {
  return std::get<std::reference_wrapper<const T>>(data_).get();
}

using ParameterArgs = std::vector<Param>;

}  // namespace parameters

NVSERV_END_NAMESPACE
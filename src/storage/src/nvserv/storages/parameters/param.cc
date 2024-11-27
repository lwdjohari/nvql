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

#include "nvserv/storages/parameters/param.h"

NVSERV_BEGIN_NAMESPACE(storages)

namespace parameters {

Param::Param(const int16_t& value)
                : data_(std::cref(value)), type_(DataType::SmallInt) {}
Param::Param(const int32_t& value)
                : data_(std::cref(value)), type_(DataType::Int) {}
Param::Param(const int64_t& value)
                : data_(std::cref(value)), type_(DataType::BigInt) {}
Param::Param(const double& value)
                : data_(std::cref(value)), type_(DataType::Double) {}
Param::Param(const float& value)
                : data_(std::cref(value)), type_(DataType::Real) {}
Param::Param(const std::string& value)
                : data_(std::cref(value)), type_(DataType::String) {}
Param::Param(const bool& value)
                : data_(std::cref(value)), type_(DataType::Boolean) {}

Param::Param(const std::chrono::system_clock::time_point& value)
                : data_(std::cref(value)), type_(DataType::Timestamp) {}
Param::Param(const NvDateTime& value)
                : data_(std::cref(value)), type_(DataType::Timestampz) {}

Param Param::SmallInt(const int16_t& value) {
  return Param(value);
}

Param Param::Int(const int32_t& value) {
  return Param(value);
}

Param Param::BigInt(const int64_t& value) {
  return Param(value);
}

Param Param::Double(const double& value) {
  return Param(value);
}

Param Param::Real(const float& value) {
  return Param(value);
}

Param Param::String(const std::string& value) {
  return Param(value);
}

Param Param::Bool(const bool& value) {
  return Param(value);
}

Param Param::Timestamp(const std::chrono::system_clock::time_point& value) {
  return Param(value);
}

Param Param::Timestampz(const NvDateTime& value) {
  return Param(value);
}

DataType Param::Type() const {
  return type_;
}

}  // namespace parameters
NVSERV_END_NAMESPACE
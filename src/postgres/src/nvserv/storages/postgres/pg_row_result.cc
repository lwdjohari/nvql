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

#include "nvserv/storages/postgres/pg_row_result.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

PgRowResult::PgRowResult(std::shared_ptr<pqxx::row> row)
                : RowResult(), row_(row) {}
PgRowResult::~PgRowResult() = default;

std::optional<Column> PgRowResult::GetColumn(
    const std::string& columnName) const  {
  try {
    return PgColumn((*row_)[columnName]);
  } catch (const std::exception& e) {
    return std::nullopt;
  }
}

std::optional<Column> PgRowResult::GetColumn(const size_t& index) const  {
  if (index >= row_->size()) {
    return std::nullopt;
  }
  return PgColumn(row_->at(index));
}

size_t PgRowResult::Size() const  {
  return row_->size();
}

// ColumnIterator begin() const {
//     return ColumnIterator(row_, 0);
// }

// ColumnIterator end() const {
//     return ColumnIterator(row_, row_.size());
// }

int16_t PgRowResult::AsImpl_int16_t(const size_t& index) const  {
  try {
    return row_->at(index).as<int16_t>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

int32_t PgRowResult::AsImpl_int32_t(const size_t& index) const  {
  try {
    return row_->at(index).as<int32_t>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

int64_t PgRowResult::AsImpl_int64_t(const size_t& index) const  {
  try {
    return row_->at(index).as<int64_t>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

std::string PgRowResult::AsImpl_string(const size_t& index) const  {
  try {
    return row_->at(index).as<std::string>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

float PgRowResult::AsImpl_float(const size_t& index) const  {
  try {
    return row_->at(index).as<float>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

double PgRowResult::AsImpl_double(const size_t& index) const  {
  try {
    return row_->at(index).as<double>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

nvm::dates::DateTime PgRowResult::AsImpl_DateTime_Timestampz(
    const size_t& index) const  {
  try {
    auto time_point =
        helper::ParseTimestampz(row_->at(index).as<std::string>());
    return nvm::dates::DateTime(time_point, "Etc/Utc");
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

nvm::dates::DateTime PgRowResult::AsImpl_DateTime_Timestamp(
    const size_t& index) const  {
  try {
    auto time_point = helper::ParseTimestamp(row_->at(index).as<std::string>());
    return nvm::dates::DateTime(time_point, "Etc/Utc");
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

int16_t PgRowResult::AsImpl_int16_t(const std::string& column_name) const  {
  try {
    return row_->at(column_name).as<int16_t>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

int32_t PgRowResult::AsImpl_int32_t(const std::string& column_name) const  {
  try {
    return row_->at(column_name).as<int32_t>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

int64_t PgRowResult::AsImpl_int64_t(const std::string& column_name) const  {
  try {
    return row_->at(column_name).as<int64_t>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

std::string PgRowResult::AsImpl_string(const std::string& column_name) const  {
  try {
    return __NR_RETURN_MOVE(row_->at(column_name).as<std::string>());
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

float PgRowResult::AsImpl_float(const std::string& column_name) const  {
  try {
    return row_->at(column_name).as<float>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

double PgRowResult::AsImpl_double(const std::string& column_name) const  {
  try {
    return row_->at(column_name).as<double>();
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

nvm::dates::DateTime PgRowResult::AsImpl_DateTime_Timestampz(
    const std::string& column_name) const  {
  try {
    auto time_point =
        helper::ParseTimestampz(row_->at(column_name).as<std::string>());
    return __NR_RETURN_MOVE(nvm::dates::DateTime(time_point, "Etc/Utc"));
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

nvm::dates::DateTime PgRowResult::AsImpl_DateTime_Timestamp(
    const std::string& column_name) const  {
  try {
    auto time_point =
        helper::ParseTimestamp(row_->at(column_name).as<std::string>());
    return __NR_RETURN_MOVE(nvm::dates::DateTime(time_point, "Etc/Utc"));
  } catch (const std::exception& e) {
    // std::cerr << e.what() << '\n';
    throw std::invalid_argument(e.what());
  }
};

NVSERV_END_NAMESPACE
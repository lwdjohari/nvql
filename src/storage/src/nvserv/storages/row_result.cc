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

#include "nvserv/storages/row_result.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

RowResult::RowResult() {}

std::optional<Column> RowResult::GetColumn(
    const std::string& column_name) const {
  return std::nullopt;
}

std::optional<Column> RowResult::GetColumn(const int& index) const {
  return std::nullopt;
}

size_t RowResult::Size() const {
  throw std::runtime_error("Implement RowResult::Size() on derrived class");
}

// ColumnIterator RowResult::begin() const {
//     return ColumnIterator(row_, 0);
// }

// ColumnIterator RowResult::end() const {
//     return ColumnIterator(row_, row_.size());
// }

NVSERV_END_NAMESPACE

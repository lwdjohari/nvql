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

#include "nvserv/storages/execution_result.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

ExecutionResult::ExecutionResult(StorageType type) : type_(type) {}
ExecutionResult::~ExecutionResult() = default;

StorageType ExecutionResult::Type() const {
  return type_;
}

Cursor::Cursor(const ExecutionResult& exec_result)
                : exec_result_(exec_result) {}

Cursor::Iterator::Iterator(std::unique_ptr<RowResultIterator> iter)
                : iter_(std::move(iter)) {}

Cursor::Iterator& Cursor::Iterator::operator++() {
  ++(*iter_);
  return *this;
}

bool Cursor::Iterator::operator!=(const Iterator& other) const {
  return *iter_ != *(other.iter_);
}

RowResultPtr Cursor::Iterator::operator*() const {
  return **iter_;
}

Cursor::Iterator Cursor::begin() const {
  return Iterator(exec_result_.begin());
}

Cursor::Iterator Cursor::end() const {
  return Iterator(exec_result_.end());
}

NVSERV_END_NAMESPACE
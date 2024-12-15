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

#include "nvserv/storages/postgres/pg_execution_result.h"

#include "nvserv/exceptions.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

PgExecutionResult::PgExecutionResult(pqxx::result&& result)
                : ExecutionResult(StorageType::Postgres),
                  result_(std::forward<pqxx::result>(result)) {}

bool PgExecutionResult::Empty() const {
  return result_.empty();
}

size_t PgExecutionResult::RowAffected() const {
  return result_.affected_rows();
}

RowResultPtr PgExecutionResult::At(const int& offset) const {
  if (offset < 0 || offset >= result_.size()) {
    throw nvserv::OutOfBoundException("`At`offset out of range [" +
                                      std::to_string(offset) + "]");
  }
  return std::move(std::make_shared<PgRowResult>(
      std::make_shared<pqxx::row>(result_.at(offset))));
}

std::unique_ptr<RowResultIterator> PgExecutionResult::begin() const {
  return std::make_unique<PgRowResultIterator>(result_, 0);
}

std::unique_ptr<RowResultIterator> PgExecutionResult::end() const {
  return std::make_unique<PgRowResultIterator>(result_, result_.size());
}

NVSERV_END_NAMESPACE
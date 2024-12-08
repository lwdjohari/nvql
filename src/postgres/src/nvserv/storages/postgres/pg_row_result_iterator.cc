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

#include "nvserv/storages/postgres/pg_row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

PgRowResultIterator::PgRowResultIterator(const pqxx::result& result, int index)
                : result_(result), index_(index) {}

PgRowResultIterator& PgRowResultIterator::operator++() {
  ++index_;
  return *this;
}

bool PgRowResultIterator::operator==(const RowResultIterator& other) const {
  auto other_pg = dynamic_cast<const PgRowResultIterator*>(&other);
  return other_pg && &result_ == &(other_pg->result_) &&
         index_ == other_pg->index_;
}

bool PgRowResultIterator::operator!=(const RowResultIterator& other) const {
  return !(*this == other);
}

RowResultPtr PgRowResultIterator::operator*() const {
  return std::make_shared<PgRowResult>(
      std::make_shared<pqxx::row>(result_.at(index_)));
}

std::unique_ptr<RowResultIterator> PgRowResultIterator::clone() const {
  return std::make_unique<PgRowResultIterator>(*this);
}

NVSERV_END_NAMESPACE
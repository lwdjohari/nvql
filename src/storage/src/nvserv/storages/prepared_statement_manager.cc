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

#include "nvserv/storages/prepared_statement_manager.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

PreparedStatementItem::PreparedStatementItem(
    const std::string& statement_key, const std::string& name,
    const std::string& query,
    const std::chrono::system_clock::time_point& created)
                : statement_key_(std::string(statement_key)),
                  name_(std::string(name)),
                  query_(std::string(query)),
                  created_time_(
                      std::chrono::system_clock::time_point(created)) {}

__NR_STRING_COMPAT_REF PreparedStatementItem::Query() const {
  return query_;
}

const std::string& PreparedStatementItem::Name() const {
  return name_;
}

const std::string& PreparedStatementItem::Key() const {
  return statement_key_;
}

std::chrono::system_clock::time_point PreparedStatementItem::CreatedTime()
    const {
  return created_time_;
}

PreparedStatementManager::PreparedStatementManager() {};
PreparedStatementManager::~PreparedStatementManager() {};

std::optional<std::pair<std::string, bool>> PreparedStatementManager::Register(
    const __NR_STRING_COMPAT_REF query) {
  if (query.empty()) {
    return std::nullopt;
  }

  if (nvm::strings::utility::IsWhitespaceString(
          __NR_CALL_STRING_COMPAT_REF(query))) {
    return std::nullopt;
  }

  auto key = hash_fn_(__NR_CALL_STRING_COMPAT_REF(query));
  auto key_str = GenerateKey(key);
  if (IsKeyExist(key_str)) {
    
    return std::move(std::make_pair(std::move(key_str), false));
  }

  statements_.emplace(
      std::string(key_str),
      std::move(PreparedStatementItem(
          key_str, std::to_string(key), std::string(query),
          nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time())));
  
  return std::move(std::make_pair(std::move(key_str), true));
}

bool PreparedStatementManager::IsKeyExist(
    const std::string& statement_key) const {
  return statements_.contains(statement_key);
}

bool PreparedStatementManager::IsQueryExist(const std::string& query) const {
  auto key = hash_fn_(query);
  return PreparedStatementManager::IsKeyExist(GenerateKey(key));
}

PreparedStatementManagerPtr PreparedStatementManager::Share() {
  return this->shared_from_this();
}

std::string PreparedStatementManager::GenerateKey(const size_t& hash) const {
  return "nvql_" + std::to_string(hash);
}

NVSERV_END_NAMESPACE

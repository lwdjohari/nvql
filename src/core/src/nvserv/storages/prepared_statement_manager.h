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

#include <absl/container/node_hash_map.h>
#include <nvm/dates/datetime.h>
#include <nvm/macro.h>
#include <nvm/strings/utility.h>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "nvserv/global_macro.h"
#include "nvserv/headers/absl_thread.h"
#include "nvserv/storages/declare.h"
NVSERV_BEGIN_NAMESPACE(storages)

class PreparedStatementItem {
 public:
  explicit PreparedStatementItem(
      const std::string& statement_key, const std::string& name,
      const std::string& query,
      const std::chrono::system_clock::time_point& created)
                  : statement_key_(std::string(statement_key)),
                    name_(name),
                    query_(std::string(query)),
                    created_time_(
                        std::chrono::system_clock::time_point(created)) {}
  __NR_STRING_COMPAT_REF Query() const {
    return query_;
  }

  const std::string& Name() const {
    return name_;
  }

  const std::string& Key() const {
    return statement_key_;
  }

  std::chrono::system_clock::time_point CreatedTime() const {
    return created_time_;
  }

 private:
  std::string statement_key_;
  std::string name_;
  std::string query_;
  std::chrono::system_clock::time_point created_time_;
};

/// @brief Managing prepared statement query, one connection will be have one
/// PreparedStatementManager.
class PreparedStatementManager : public std::enable_shared_from_this<
                                     PreparedStatementManager> {
 public:
  PreparedStatementManager(){};
  ~PreparedStatementManager(){};

  std::optional<std::pair<std::string, bool>> Register(
      const __NR_STRING_COMPAT_REF query) {
    if (query.empty())
      return std::nullopt;

    if (nvm::strings::utility::IsWhitespaceString(
            __NR_CALL_STRING_COMPAT_REF(query)))
      return std::nullopt;

    auto key = hash_fn_(__NR_CALL_STRING_COMPAT_REF(query));
    auto key_str = GenerateKey(key);
    if (IsKeyExist(key_str))
      return __NR_RETURN_MOVE(std::make_pair(key_str, false));

    statements_.emplace(
        key_str,
        std::move(PreparedStatementItem(
            key_str, std::string(std::to_string(key)), std::string(query),
            nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time())));
    return __NR_RETURN_MOVE(std::make_pair(key_str, true));
  }

  // std::optional<size_t> Register(const std::string& name,
  //                                const std::string& query) {
  //   if (query.empty())
  //     return std::nullopt;

  //   if (nvm::strings::utility::IsWhitespaceString(query))
  //     return std::nullopt;

  //   auto key = hash_fn_(query);

  //   if (IsKeyExist(GenerateKey(key)))
  //     return false;

  //   statements_.emplace(
  //       key_str, std::move(PreparedStatementItem(
  //                key_str, std::string(name), std::string(query),
  //                nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time())));
  //   return key;
  // }

  bool IsKeyExist(const std::string& statement_key) const {
    return statements_.contains(statement_key);
  }

  bool IsQueryExist(const std::string& query) const {
    auto key = hash_fn_(query);
    return IsKeyExist(GenerateKey(key));
  }

  PreparedStatementManagerPtr Share() {
    return this->shared_from_this();
  }

 private:
  absl::node_hash_map<std::string, PreparedStatementItem> statements_;
  std::hash<std::string> hash_fn_;
  absl::Mutex mutex_;

  std::string GenerateKey(const size_t& hash) const {
    return "nvql_" + std::to_string(hash);
  }
};

NVSERV_END_NAMESPACE

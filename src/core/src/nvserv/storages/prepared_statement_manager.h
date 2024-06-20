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
      const std::chrono::system_clock::time_point& created);

  __NR_STRING_COMPAT_REF Query() const;

  const std::string& Name() const;

  const std::string& Key() const;

  std::chrono::system_clock::time_point CreatedTime() const;

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
  PreparedStatementManager();
  ~PreparedStatementManager();

  std::optional<std::pair<std::string, bool>> Register(
      const __NR_STRING_COMPAT_REF query);

  bool IsKeyExist(const std::string& statement_key) const;

  bool IsQueryExist(const std::string& query) const;

  PreparedStatementManagerPtr Share();

 private:
  absl::node_hash_map<std::string, PreparedStatementItem> statements_;
  std::hash<std::string> hash_fn_;
  absl::Mutex mutex_;

  std::string GenerateKey(const size_t& hash) const;
};

NVSERV_END_NAMESPACE

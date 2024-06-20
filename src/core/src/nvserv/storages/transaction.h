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
#include <ostream>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/execution_result.h"
#include "nvserv/storages/parameters/param.h"
#include "nvserv/storages/row_result_iterator.h"
NVSERV_BEGIN_NAMESPACE(storages)

class Transaction {
 public:
  Transaction(StorageType type, TransactionMode mode);

  virtual ~Transaction();

  /// @brief Execute SQL statement using NvQL. Behind-scenes NvQL data layer
  /// will execute using prepared statements and parameters. NvQL will manages
  /// all the wirings for using prepared statements and the binary transport
  /// protocol if supported by storage server.
  /// @tparam ...Args
  /// @param query
  /// @param ...args
  /// @return
  template <typename... Args>
  [[nodiscard]] ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query,
                                           const Args&... args);

  // Execute query string
  // Behind-scene NvQL will wiring and manager the prepare statment
  // and will be execute using prepared statement
  [[nodiscard]] ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query);

  // Execute query string with parameters
  // Behind-scene NvQL will wiring and manager the prepare statment
  // and will be execute using prepared statement and send the parameters
  [[nodiscard]] ExecutionResultPtr Execute(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args);

  // Execute query and send the query string
  // Bypass the prepared statement routines
  [[nodiscard]] ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args);

  // Execute query and send the query string
  // Bypass the prepared statement routines
  [[nodiscard]] ExecutionResultPtr ExecuteNonPrepared(
      const __NR_STRING_COMPAT_REF query);

  const StorageType& Type() const;

  const TransactionMode& Mode() const;

  virtual void Commit() = 0;
  virtual void Rollback() = 0;

 protected:
  StorageType type_;
  TransactionMode mode_;

  virtual ExecutionResultPtr ExecuteImpl(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) = 0;

  virtual ExecutionResultPtr ExecuteNonPreparedImpl(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) = 0;
};

template <typename... Args>
[[nodiscard]] ExecutionResultPtr Transaction::Execute(
    const __NR_STRING_COMPAT_REF query, const Args&... args) {
  std::vector<parameters::Param> params = {args...};
  return ExecuteImpl(query, params);
}

NVSERV_END_NAMESPACE
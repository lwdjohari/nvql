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

#include "nvserv/storages/transaction.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

Transaction::Transaction(StorageType type, TransactionMode mode)
                : type_(type), mode_(mode){};

Transaction::~Transaction(){};

[[nodiscard]] ExecutionResultPtr Transaction::Execute(
    const __NR_STRING_COMPAT_REF query) {
  return ExecuteImpl(query, parameters::ParameterArgs());
}

[[nodiscard]] ExecutionResultPtr Transaction::Execute(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  return ExecuteImpl(query, args);
}

[[nodiscard]] ExecutionResultPtr Transaction::ExecuteNonPrepared(
    const __NR_STRING_COMPAT_REF query, const parameters::ParameterArgs& args) {
  return ExecuteNonPreparedImpl(query, args);
}

[[nodiscard]] ExecutionResultPtr Transaction::ExecuteNonPrepared(
    const __NR_STRING_COMPAT_REF query) {
  return ExecuteNonPreparedImpl(query, parameters::ParameterArgs());
}

const StorageType& Transaction::Type() const {
  return type_;
}

const TransactionMode& Transaction::Mode() const {
  return mode_;
}

NVSERV_END_NAMESPACE
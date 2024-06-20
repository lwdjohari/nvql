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

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/exceptions.h"

NVSERV_BEGIN_NAMESPACE(storages)

class StorageException : public Exception {
 public:
  explicit StorageException(const std::string& message, const StorageType& type)
                  : Exception(message), type_(StorageType(type)) {}

  const StorageType& Type() const {
    return type_;
  }

 private:
  StorageType type_;
};

class ConnectionException : public StorageException {
 public:
  explicit ConnectionException(const std::string& message,
                               const StorageType& type)
                  : StorageException(message, type) {}
};

class TransactionException : StorageException {
 public:
  explicit TransactionException(const std::string& message,
                                const StorageType& type)
                  : StorageException(message, type) {}
};

class ExecutionException : TransactionException {
 public:
  explicit ExecutionException(const std::string& message,
                              const StorageType& type)
                  : TransactionException(message, type) {}
};

class InternalErrorException : StorageException {
 public:
  explicit InternalErrorException(const std::string& message,
                                  const StorageType& type)
                  : StorageException(message, type) {}
};

class ParameterTypeException : StorageException {
 public:
  explicit ParameterTypeException(const std::string& message,
                                  const StorageType& type)
                  : StorageException(message, type) {}
};

class UnsupportedFeatureException : StorageException {
 public:
  explicit UnsupportedFeatureException(const std::string& message,
                                       const StorageType& type)
                  : StorageException(message, type) {}
};

NVSERV_END_NAMESPACE
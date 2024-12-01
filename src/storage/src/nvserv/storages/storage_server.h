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

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0
#include "nvserv/components/component.h"
#endif

#include "nvserv/global_macro.h"
#include "nvserv/storages/cluster_config.h"
#include "nvserv/storages/connection_pool.h"
#include "nvserv/storages/declare.h"

NVSERV_BEGIN_NAMESPACE(storages)

#if not defined(NVQL_STANDALONE) || NVQL_STANDALONE == 0

class StorageServer : public components::Component {
 public:
  explicit StorageServer(components::ComponentType type)
                  : components::Component(type) {};

  explicit StorageServer(const components::ComponentLocator& locator,
                         const components::ComponentConfig& config,
                         components::ComponentType type)
                  : components::Component(locator, config, type) {};

#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  class StorageServer {
   public:
    StorageServer() {};
#endif

    virtual ~StorageServer() {};

    virtual const std::string& Name() const = 0;

    virtual const StorageConfig& Configs() const = 0;

    virtual bool TryConnect() = 0;

    virtual bool Shutdown(
        bool grace_shutdown = true,
        std::chrono::seconds deadline = std::chrono::seconds(0)) = 0;

    virtual TransactionPtr Begin(
        TransactionMode mode = TransactionMode::ReadWrite) = 0;

    virtual const ConnectionPoolPtr Pool() const = 0;

    virtual ConnectionPoolPtr Pool() = 0;

    virtual StorageInfo GetStorageServerInfo() const = 0;
  };

  NVSERV_END_NAMESPACE
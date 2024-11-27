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

#include <nvm/types/type_utility.h>

#include <ostream>
#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ClusterConfigList {
 public:
  explicit ClusterConfigList(StorageType type);

  template <typename T>
  void Add(T&& config);

  const ClusterConfigListType & Configs() const;

  ClusterConfigListType & Configs();
  size_t Size() const;

  StorageType Type() const;
  

 protected:
  ClusterConfigListType configs_;
  StorageType type_;
};


template <typename T>
  void ClusterConfigList::Add(T&& config) {
    using namespace nvm::types::utility;

    static_assert(is_has_base_of_v<T, ClusterConfig>,
                  "Type must be derrived type from \"ClusterConfig\"");

    if (config.Type() != type_)
      throw std::invalid_argument(
          "ClusterConfigList is set to only accept StorageType = " +
          ToStringEnumStorageType(type_));

    configs_.push_back(std::make_shared<T>(std::forward<T>(config)));
  };

NVSERV_END_NAMESPACE
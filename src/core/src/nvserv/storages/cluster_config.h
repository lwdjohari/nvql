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

class ClusterConfigBase {
 public:
  virtual StorageType Type() const = 0;
  virtual const std::string& User() const = 0;
  virtual const std::string& Password() const = 0;
  virtual const std::string& Host() const = 0;
  virtual std::string GetConfig() const = 0;
  virtual uint32_t Port() const = 0;
};


class ClusterConfig : public ClusterConfigBase {
 public:
  virtual ~ClusterConfig();

  const std::string& User() const override;

  const std::string& Password() const override;

  const std::string& Host() const override;

  uint32_t Port() const override;

  StorageType Type() const override;

  virtual std::string GetConfig() const override;

 protected:
  explicit ClusterConfig(StorageType type, const std::string& user,
                         const std::string& password, const std::string& host,
                         const uint32_t port);

 private:
  std::string user_;
  std::string password_;
  std::string host_;
  uint32_t port_;
  StorageType type_;
};





NVSERV_END_NAMESPACE
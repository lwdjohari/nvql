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

#include <ostream>
#include <pqxx/pqxx>

#include "nvserv/global_macro.h"
#include "nvserv/storages/cluster_config.h"
#include "nvserv/storages/storage_config.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgClusterConfig : public storages::ClusterConfig {
 private:
  //   std::vector<std::string> cluster_conninfo = {
  //       "dbname=test user=postgres password=secret hostaddr=192.168.1.1 "
  //       "port=5432",
  //       "dbname=test user=postgres password=secret hostaddr=192.168.1.2 "
  //       "port=5432",
  //       "dbname=test user=postgres password=secret hostaddr=192.168.1.3 "
  //       "port=5432"};

  std::string dbname_;

 public:
  explicit PgClusterConfig(const std::string& dbname, const std::string& user,
                           const std::string& password, const std::string& host,
                           uint32_t port)
                  : ClusterConfig(StorageType::Postgres, user, password, host,
                                  port),
                    dbname_(dbname){};

  const __NR_STRING_COMPAT_REF DbName() const {
    return dbname_;
  }

  std::string GetConfig() const override {
    return __NR_RETURN_MOVE(std::ostringstream().str());
  }
};

NVSERV_END_NAMESPACE
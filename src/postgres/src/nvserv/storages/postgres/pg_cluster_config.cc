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

#include "nvserv/storages/postgres/pg_cluster_config.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

PgClusterConfig::PgClusterConfig(const std::string& dbname, const std::string& user,
                           const std::string& password, const std::string& host,
                           uint32_t port)
                  : ClusterConfig(StorageType::Postgres, user, password, host,
                                  port),
                    dbname_(dbname){};

   __NR_STRING_COMPAT_REF PgClusterConfig::DbName() const {
    return dbname_;
  }

  std::string PgClusterConfig::GetConfig() const  {
    return __NR_RETURN_MOVE(std::ostringstream().str());
  }

NVSERV_END_NAMESPACE
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

#include "nvserv/storages/connection.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

ConnectionBase::ConnectionBase(){};

Connection::Connection(const std::string& name, StorageType type,
                       ConnectionStandbyMode standby_mode,
                       std::chrono::seconds mark_idle_after)
                : ConnectionBase(),
                  name_(std::string(name)),
                  prepared_statement_manager_(
                      std::make_shared<PreparedStatementManager>()),
                  created_(
                      nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time()),
                  acquired_(created_),
                  returned_(created_),
                  last_ping_(created_),
                  mark_idle_after_(mark_idle_after),
                  type_(type),
                  standby_mode_(standby_mode) {}

StorageType Connection::Type() const {
  return type_;
}

const std::string& Connection::Name() const {
  return name_;
}

///< Write state when when connection being acquired to pool
void Connection::Acquire() {
  absl::MutexLock lock(&mutex_);
  acquired_ = nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time();
}

///< Write state when when connection being returned to pool
void Connection::Returned() {
  absl::MutexLock lock(&mutex_);
  returned_ = nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time();
}

/// Return time when the Connection object was created
std::chrono::system_clock::time_point Connection::CreatedTime() const {
  return created_;
}

///< Time when connection being acquired from pool
std::chrono::system_clock::time_point Connection::AcquiredTime() const {
  return acquired_;
}

///< Time  when connection being acquired from pool
std::chrono::system_clock::time_point Connection::ReturnedTime() const {
  return returned_;
}

PreparedStatementManagerPtr Connection::PreparedStatement() {
  return prepared_statement_manager_;
}

bool Connection::IsIdle() const {
  auto idle = IdleDuration();
  return idle > mark_idle_after_ ? true : false;
}

/// Duration since last usage
std::chrono::seconds Connection::IdleDuration() const {
  auto now = nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time();
  return std::chrono::duration_cast<std::chrono::seconds>(now - returned_);
}

std::chrono::seconds Connection::IdleAfter() const {
  return mark_idle_after_;
}

std::chrono::system_clock::time_point Connection::LastPing() const {
  return last_ping_;
}

ConnectionStandbyMode Connection::StandbyMode() const {
  return standby_mode_;
}

NVSERV_END_NAMESPACE
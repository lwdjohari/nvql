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

#include <nvm/dates/datetime.h>

#include <chrono>

#include "nvserv/global_macro.h"
#include "nvserv/headers/absl_thread.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/prepared_statement_manager.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ConnectionBase {
 public:
  // virtual ~Connection() = default;
  virtual TransactionPtr CreateTransaction() = 0;
  virtual void Open() = 0;
  virtual void Close() = 0;

  ///< Ping server to report keep-alive.
  /// Different storage server has different implementations
  /// to guarantee the connection is keep-alive
  virtual void PingServerAsync() = 0;
  virtual bool PingServer() = 0;
  virtual std::chrono::system_clock::time_point LastPing() const = 0;

  virtual ConnectionStandbyMode StandbyMode() const = 0;
  virtual const std::string& Name() const = 0;
  ///< Write state when when connection being acquired to pool
  virtual void Acquire() = 0;

  ///< Write state when when connection being returned to pool
  virtual void Returned() = 0;

  /// Return time when the Connection object was created
  virtual std::chrono::system_clock::time_point CreatedTime() const = 0;

  /// Return time when the Connection object acquired from pool
  virtual std::chrono::system_clock::time_point AcquiredTime() const = 0;

  /// Return time when the Connection object returned to pool
  virtual std::chrono::system_clock::time_point ReturnedTime() const = 0;

  /// Duration since last usage
  virtual std::chrono::seconds IdleDuration() const = 0;

  virtual std::chrono::seconds IdleAfter() const = 0;

  virtual TransactionMode SupportedTransactionMode() const = 0;

  virtual void ReportHealth() const = 0;

  virtual bool IsOpen() const = 0;

  virtual ConnectionMode Mode() const = 0;

  virtual bool IsIdle() const = 0;

  virtual void Release() = 0;

  virtual size_t GetHash() const = 0;

  virtual StorageType Type() const = 0;

  virtual const std::string& GetConnectionString() const = 0;

  virtual std::optional<std::pair<std::string, bool>> PrepareStatement(
      __NR_STRING_COMPAT_REF query) = 0;
  virtual PreparedStatementManagerPtr PreparedStatement() = 0;

 protected:
  ConnectionBase();
};

class Connection : public ConnectionBase {
 public:
  StorageType Type() const override;

  const std::string& Name() const override;

  ///< Write state when when connection being acquired to pool
  void Acquire() override;

  ///< Write state when when connection being returned to pool
  void Returned() override;

  /// Return time when the Connection object was created
  std::chrono::system_clock::time_point CreatedTime() const override;

  ///< Time when connection being acquired from pool
  std::chrono::system_clock::time_point AcquiredTime() const override;

  ///< Time  when connection being acquired from pool
  std::chrono::system_clock::time_point ReturnedTime() const override;

  PreparedStatementManagerPtr PreparedStatement() override;

  bool IsIdle() const override;

  /// Duration since last usage
  std::chrono::seconds IdleDuration() const override;

  std::chrono::seconds IdleAfter() const override;

  std::chrono::system_clock::time_point LastPing() const override;

  ConnectionStandbyMode StandbyMode() const override;

 protected:
  explicit Connection(
      const std::string& name, StorageType type,
      ConnectionStandbyMode standby_mode,
      std::chrono::seconds mark_idle_after = std::chrono::seconds(300));

  std::string name_;
  mutable absl::Mutex mutex_;
  PreparedStatementManagerPtr prepared_statement_manager_;
  std::chrono::system_clock::time_point created_;
  std::chrono::system_clock::time_point acquired_;
  std::chrono::system_clock::time_point returned_;
  std::chrono::system_clock::time_point last_ping_;
  std::chrono::seconds mark_idle_after_;
  StorageType type_;
  ConnectionStandbyMode standby_mode_;

  virtual void OpenImpl() = 0;
  virtual void CloseImpl() = 0;
  virtual void ReleaseImpl() = 0;
};

NVSERV_END_NAMESPACE
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

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
#pragma once

#include <absl/container/btree_map.h>
#include <absl/synchronization/mutex.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>

#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include "nvserv/global_macro.h"

NVSERV_BEGIN_NAMESPACE(threads)

/// @brief Single thread event loop executor.
/// Support RunOnce and RunAtInterval task, when found task with same time  the
/// task will be executed sequenly.
class EventLoopExecutor {
 public:
  enum class TaskType { None, RunOnce, RunAtInterval };

  using Task = std::function<void()>;

  EventLoopExecutor();

  ~EventLoopExecutor();

  void SubmitTask(Task task, TaskType type, absl::Duration deadline,
                  absl::Duration interval = absl::ZeroDuration());

 private:
  struct TaskItem {
    Task task;
    TaskType type;
    absl::Duration interval;
  };

  std::thread thread_;
  absl::Mutex mutex_;
  absl::CondVar cond_var_;
  absl::btree_multimap<absl::Time, TaskItem> tasks_;
  bool stop_;

  void RunLoop();
};

NVSERV_END_NAMESPACE

#endif
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

#include "nvserv/threads/event_loop_executor.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(threads)

/// @brief Single thread event loop executor.
/// Support RunOnce and RunAtInterval task, when found task with same time  the
/// task will be executed sequenly.

EventLoopExecutor::EventLoopExecutor() : stop_(false) {
  absl::MutexLock lock(&mutex_);
  thread_ = std::thread(&EventLoopExecutor::RunLoop, this);
  cond_var_.Wait(&mutex_);
}

EventLoopExecutor::~EventLoopExecutor() {
  {
    absl::MutexLock lock(&mutex_);
    stop_ = true;
    cond_var_.SignalAll();
  }
  if (thread_.joinable()) {
    thread_.join();
  }
}

void EventLoopExecutor::SubmitTask(
    Task task, TaskType type, absl::Duration deadline,
    absl::Duration interval) {
  absl::MutexLock lock(&mutex_);
  tasks_.emplace(absl::Now() + deadline,
                 TaskItem{std::move(task), type, interval});
  // Notify the waiting thread that a new task has been submitted.
  cond_var_.SignalAll();
}

void EventLoopExecutor::RunLoop() {
  cond_var_.SignalAll();
  while (true) {
    std::vector<Task> tasks_to_run;
    std::vector<std::pair<absl::Time, TaskItem>> reschedule_tasks;
    absl::Time next_deadline = absl::InfiniteFuture();

    {
      absl::MutexLock lock(&mutex_);
      if (stop_ && tasks_.empty())
        break;

      auto now = absl::Now();

      if (tasks_.empty()) {
        // Wait indefinitely until a new task is submitted.
        cond_var_.Wait(&mutex_);
      } else {
        next_deadline = tasks_.begin()->first;
        if (next_deadline <= now) {
          cond_var_.SignalAll();
        } else {
          cond_var_.WaitWithDeadline(&mutex_, next_deadline);
        }
      }

      now = absl::Now();
      while (!tasks_.empty() && tasks_.begin()->first <= now) {
        tasks_to_run.push_back(tasks_.begin()->second.task);
        if (tasks_.begin()->second.type == TaskType::RunAtInterval) {
          reschedule_tasks.emplace_back(now + tasks_.begin()->second.interval,
                                        tasks_.begin()->second);
        }
        tasks_.erase(tasks_.begin());
      }
    }

    for (auto& task : tasks_to_run) {
      absl::Time task_start = absl::Now();
      try {
        task();
      } catch (const std::exception& e) {
        std::cerr << "Task execution error: " << e.what() << std::endl;
      } catch (...) {
        std::cerr << "Unknown task execution error." << std::endl;
      }
      absl::Time task_end = absl::Now();
      absl::Duration task_duration = task_end - task_start;

      if (stop_) {
        // Exit if stop_ is set after finishing current tasks
        return;
      }

      if (!tasks_.empty() &&
          task_duration >= (tasks_.begin()->first - task_start)) {
        next_deadline = tasks_.begin()->first;
        absl::MutexLock lock(&mutex_);
        // Notify the condition variable in case of overrunning task.
        cond_var_.SignalAll();
      }
    }

    {
      absl::MutexLock lock(&mutex_);
      if (stop_) {
        // Don't reschedule tasks if stopping
        break;
      }
      for (const auto& item : reschedule_tasks) {
        tasks_.emplace(item.first, item.second);
      }
    }
  }
}

NVSERV_END_NAMESPACE

#endif
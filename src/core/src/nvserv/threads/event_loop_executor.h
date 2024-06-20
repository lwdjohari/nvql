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

  EventLoopExecutor() : stop_(false) {
    absl::MutexLock lock(&mutex_);
    thread_ = std::thread(&EventLoopExecutor::RunLoop, this);
    cond_var_.Wait(&mutex_);
  }

  ~EventLoopExecutor() {
    {
      absl::MutexLock lock(&mutex_);
      stop_ = true;
      cond_var_.SignalAll();
    }
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void SubmitTask(Task task, TaskType type, absl::Duration deadline,
                  absl::Duration interval = absl::ZeroDuration()) {
    absl::MutexLock lock(&mutex_);
    tasks_.emplace(absl::Now() + deadline,
                   TaskItem{std::move(task), type, interval});
    // Notify the waiting thread that a new task has been submitted.
    cond_var_.SignalAll();
  }

 private:
  struct TaskItem {
    Task task;
    TaskType type;
    absl::Duration interval;
  };

  void RunLoop() {
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

  std::thread thread_;
  absl::Mutex mutex_;
  absl::CondVar cond_var_;
  absl::btree_multimap<absl::Time, TaskItem> tasks_;
  bool stop_;
};

int main() {
  EventLoopExecutor executor;

  executor.SubmitTask([]() { std::cout << "Initializer" << std::endl; },
                      EventLoopExecutor::TaskType::RunOnce,
                      absl::ZeroDuration());

  executor.SubmitTask([]() { std::cout << "Ping Service" << std::endl; },
                      EventLoopExecutor::TaskType::RunAtInterval,
                      absl::Milliseconds(400), absl::Milliseconds(200));

  executor.SubmitTask([]() { std::cout << "Cleaner Service" << std::endl; },
                      EventLoopExecutor::TaskType::RunAtInterval,
                      absl::Milliseconds(400), absl::Milliseconds(200));

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  return 0;
}

NVSERV_END_NAMESPACE

#endif
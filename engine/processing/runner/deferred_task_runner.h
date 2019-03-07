/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INK_ENGINE_PROCESSING_RUNNER_DEFERRED_TASK_RUNNER_H_
#define INK_ENGINE_PROCESSING_RUNNER_DEFERRED_TASK_RUNNER_H_

#include <memory>
#include <queue>

#include "ink/engine/processing/runner/task_runner.h"
#include "ink/engine/scene/frame_state/frame_state.h"
#include "ink/engine/service/dependencies.h"

namespace ink {

// A task runner that queues tasks to be run later. Subclasses must implement
// RequestServicingOfTaskQueue(), which is expected to result in a synchronous
// call to RunDeferredTasks() at some later time.
// Acquires a framerate lock when a task is pushed, and releases it in
// ServiceMainThreadTasks() when no tasks remain.
class DeferredTaskRunner : public ITaskRunner {
 public:
  explicit DeferredTaskRunner(std::shared_ptr<FrameState> frame_state)
      : frame_state_(std::move(frame_state)) {}

  void PushTask(std::unique_ptr<Task> task) override;
  void ServiceMainThreadTasks() override;
  int NumPendingTasks() const override {
    return deferred_tasks_.size() + post_execute_tasks_.size();
  }

  // Runs the Execute() method on tasks that have been queued up, in the order
  // that they were received.
  void RunDeferredTasks();

 private:
  // Used to indicate that a task has been queued, and that RunDeferredTasks()
  // should be called to perform the work.
  virtual void RequestServicingOfTaskQueue() = 0;

  std::queue<TaskWrapper> deferred_tasks_;
  std::queue<TaskWrapper> post_execute_tasks_;
  std::shared_ptr<FrameState> frame_state_;
  std::unique_ptr<FramerateLock> framelock_;
};

}  // namespace ink
#endif  // INK_ENGINE_PROCESSING_RUNNER_DEFERRED_TASK_RUNNER_H_

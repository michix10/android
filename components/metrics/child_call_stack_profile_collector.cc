// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/metrics/child_call_stack_profile_collector.h"

#include <utility>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/logging.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_task_runner_handle.h"
#include "services/service_manager/public/cpp/interface_provider.h"

namespace metrics {

ChildCallStackProfileCollector::ProfileState::ProfileState() = default;
ChildCallStackProfileCollector::ProfileState::ProfileState(ProfileState&&) =
    default;

ChildCallStackProfileCollector::ProfileState::ProfileState(
    const CallStackProfileParams& params,
    base::TimeTicks start_timestamp,
    base::StackSamplingProfiler::CallStackProfile profile)
    : params(params),
      start_timestamp(start_timestamp),
      profile(std::move(profile)) {}

ChildCallStackProfileCollector::ProfileState::~ProfileState() = default;

// Some versions of GCC need this for push_back to work with std::move.
ChildCallStackProfileCollector::ProfileState&
ChildCallStackProfileCollector::ProfileState::operator=(ProfileState&&) =
    default;

ChildCallStackProfileCollector::ChildCallStackProfileCollector() {}

ChildCallStackProfileCollector::~ChildCallStackProfileCollector() {}

base::StackSamplingProfiler::CompletedCallback
ChildCallStackProfileCollector::GetProfilerCallback(
    const CallStackProfileParams& params,
    base::TimeTicks profile_start_time) {
  return base::Bind(&ChildCallStackProfileCollector::Collect,
                    // This class has lazy instance lifetime.
                    base::Unretained(this), params, profile_start_time);
}

void ChildCallStackProfileCollector::SetParentProfileCollector(
    metrics::mojom::CallStackProfileCollectorPtr parent_collector) {
  base::AutoLock alock(lock_);
  // This function should only invoked once, during the mode of operation when
  // retaining profiles after construction.
  DCHECK(retain_profiles_);
  retain_profiles_ = false;
  task_runner_ = base::ThreadTaskRunnerHandle::Get();
  // This should only be set one time per child process.
  DCHECK(!parent_collector_);
  parent_collector_ = std::move(parent_collector);
  if (parent_collector_) {
    for (ProfileState& state : profiles_) {
      parent_collector_->Collect(state.params, state.start_timestamp,
                                 std::move(state.profile));
    }
  }
  profiles_.clear();
}

void ChildCallStackProfileCollector::Collect(
    const CallStackProfileParams& params,
    base::TimeTicks start_timestamp,
    CallStackProfile profile) {
  // Impl function is used as it needs to PostTask() to itself on a different
  // thread - which only works with a void return value.
  CollectImpl(params, start_timestamp, std::move(profile));
}

void ChildCallStackProfileCollector::CollectImpl(
    const CallStackProfileParams& params,
    base::TimeTicks start_timestamp,
    CallStackProfile profile) {
  base::AutoLock alock(lock_);
  if (task_runner_ &&
      // The profiler thread does not have a task runner. Attempting to
      // invoke Get() on it results in a DCHECK.
      (!base::ThreadTaskRunnerHandle::IsSet() ||
       base::ThreadTaskRunnerHandle::Get() != task_runner_)) {
    // Post back to the thread that owns the the parent interface.
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&ChildCallStackProfileCollector::CollectImpl,
                                  // This class has lazy instance lifetime.
                                  base::Unretained(this), params,
                                  start_timestamp, std::move(profile)));
    return;
  }

  if (parent_collector_) {
    parent_collector_->Collect(params, start_timestamp, std::move(profile));
  } else if (retain_profiles_) {
    profiles_.push_back(
        ProfileState(params, start_timestamp, std::move(profile)));
  }
}

}  // namespace metrics


#include "./task.h"

namespace thread_lib_xjturm {

Task::~Task() {
  // TODO(gaschler): Relax some checks after testing.
  if (state_ != NEW && state_ != COMPLETED) {
    //LOG(WARNING) << "Delete Task between dispatch and completion.";
  }
}

Task::State Task::GetState() {
  MutexLocker locker(&mutex_);
  return state_;
}

void Task::SetWorkItem(const WorkItem& work_item) {
  MutexLocker locker(&mutex_);
  work_item_ = work_item;
}

void Task::AddDependency(std::weak_ptr<Task> dependency) {
  std::shared_ptr<Task> shared_dependency;
  {
    MutexLocker locker(&mutex_);
    if ((shared_dependency = dependency.lock())) {
      ++uncompleted_dependencies_;
    }
  }
  if (shared_dependency) {
    shared_dependency->AddDependentTask(this);
  }
}

void Task::SetThreadPool(ThreadPoolInterface* thread_pool) {
  MutexLocker locker(&mutex_);
  state_ = DISPATCHED;
  thread_pool_to_notify_ = thread_pool;
  if (uncompleted_dependencies_ == 0) {
    state_ = DEPENDENCIES_COMPLETED;
    //CHECK(thread_pool_to_notify_);
    thread_pool_to_notify_->NotifyDependenciesCompleted(this);
  }
}

void Task::AddDependentTask(Task* dependent_task) {
  MutexLocker locker(&mutex_);
  if (state_ == COMPLETED) {
    dependent_task->OnDependenyCompleted();
    return;
  }
  bool inserted = dependent_tasks_.insert(dependent_task).second;
}

void Task::OnDependenyCompleted() {
  MutexLocker locker(&mutex_);
  --uncompleted_dependencies_;
  if (uncompleted_dependencies_ == 0 && state_ == DISPATCHED) {
    state_ = DEPENDENCIES_COMPLETED;
    thread_pool_to_notify_->NotifyDependenciesCompleted(this);
  }
}

void Task::Execute() {
  {
    MutexLocker locker(&mutex_);
    state_ = RUNNING;
  }

  // Execute the work item.
  if (work_item_) {
    work_item_();
  }

  MutexLocker locker(&mutex_);
  state_ = COMPLETED;
  for (Task* dependent_task : dependent_tasks_) {
    dependent_task->OnDependenyCompleted();
  }
}
}

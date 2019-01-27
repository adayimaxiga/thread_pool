
#ifndef COMMON_TASK_H_
#define COMMON_TASK_H_

#include <set>

#include "./mutex.h"
#include "thread_pool.h"

namespace thread_lib_xjturm {


class ThreadPoolInterface;

class Task {
 public:
  friend class ThreadPoolInterface;

  using WorkItem = std::function<void()>;
  enum State { NEW, DISPATCHED, DEPENDENCIES_COMPLETED, RUNNING, COMPLETED };

  Task() = default;
  ~Task();

  State GetState() EXCLUDES(mutex_);

  // State must be 'NEW'.
  void SetWorkItem(const WorkItem& work_item) EXCLUDES(mutex_);

  // State must be 'NEW'. 'dependency' may be nullptr, in which case it is
  // assumed completed.
  void AddDependency(std::weak_ptr<Task> dependency) EXCLUDES(mutex_);

 private:
  // Allowed in all states.
  void AddDependentTask(Task* dependent_task);

  // State must be 'DEPENDENCIES_COMPLETED' and becomes 'COMPLETED'.
  void Execute() EXCLUDES(mutex_);

  // State must be 'NEW' and becomes 'DISPATCHED' or 'DEPENDENCIES_COMPLETED'.
  void SetThreadPool(ThreadPoolInterface* thread_pool) EXCLUDES(mutex_);

  // State must be 'NEW' or 'DISPATCHED'. If 'DISPATCHED', may become
  // 'DEPENDENCIES_COMPLETED'.
  void OnDependenyCompleted();

  WorkItem work_item_ GUARDED_BY(mutex_);
  ThreadPoolInterface* thread_pool_to_notify_ GUARDED_BY(mutex_) = nullptr;
  State state_ GUARDED_BY(mutex_) = NEW;
  unsigned int uncompleted_dependencies_ GUARDED_BY(mutex_) = 0;
  std::set<Task*> dependent_tasks_ GUARDED_BY(mutex_);

  Mutex mutex_;
};

}


#endif  // COMMON_TASK_H_

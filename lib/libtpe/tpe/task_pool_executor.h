#include <iostream>
#include <vector>
#include <memory>

#include "../../libtpe/tpe/task/task_base.h"
#include "utils/concurrency/queue_thread.h"
#include "utils/patterns/singleton.h"

#ifndef SRC_TASK_POOL_EXECUTOR_H_
#define SRC_TASK_POOL_EXECUTOR_H_

namespace task {
namespace pool {

class TaskPoolExecutor:
    public  utils::patterns::Singleton<TaskPoolExecutor> {

  friend class utils::patterns::Singleton<TaskPoolExecutor>;

public:

  void init(void);

  void enqueue_task(std::shared_ptr<task::TaskBase> task);

//  void print_thread_status(void);

protected:
  TaskPoolExecutor();

private:
  void _tsk_dequeued(std::shared_ptr<task::TaskBase> tsk);

  bool _is_initialized;
  std::vector<std::unique_ptr<utils::concurrency::QueueThread<
    task::TaskBase>>> _thread_pool;
  volatile int _current_thread_idx;
};
} // pool
} // task

#endif // SRC_TASK_POOL_EXECUTOR_H_

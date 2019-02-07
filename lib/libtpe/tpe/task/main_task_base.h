#include "tpe/task/sub_task/sub_task_base.h"
#include <utils/concurrency/thread_pool.h>

#ifndef TPE_TASK_MAIN_TASK_BASE_H_
#define TPE_TASK_MAIN_TASK_BASE_H_

namespace task {
typedef utils::concurrency::ThreadPool<SubTaskBase> SubPool_t;

class MainTaskBase : public TaskBase {
public:
  MainTaskBase();

  void execute(SubPool_t& sub_pool);

protected:
  virtual void _execute(SubPool_t& sub_pool) = 0;
};

} // task

#endif /* TPE_TASK_MAIN_TASK_BASE_H_ */

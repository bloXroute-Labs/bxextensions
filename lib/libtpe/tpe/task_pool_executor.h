#include <iostream>
#include <vector>
#include <memory>

#include <utils/concurrency/queue_thread.h>
#include <utils/patterns/singleton.h>

#include "tpe/task/main_task_base.h"

#ifndef TPE_TASK_POOL_EXECUTOR_H_
#define TPE_TASK_POOL_EXECUTOR_H_

namespace task {
namespace pool {

typedef TaskThreadPool<MainTaskBase> MainPool_t;

class TaskPoolExecutor {

public:
    TaskPoolExecutor();

    void init(size_t pool_size);
    void enqueue_task(std::shared_ptr<MainTaskBase> task);

    size_t size() const;

private:

    bool _is_initialized;
    SubPool_t _sub_pool;
    MainPool_t _main_pool;
};
} // pool
} // task

#endif // TPE_TASK_POOL_EXECUTOR_H_

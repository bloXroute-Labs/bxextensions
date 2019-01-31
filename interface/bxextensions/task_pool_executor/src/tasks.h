#include "src/tasks/task_base.h"

#include <tpe/task_pool_executor.h>

#ifndef SRC_TASKS_H_
#define SRC_TASKS_H_

typedef task::pool::TaskPoolExecutor TaskPoolExecutor_t;

void bind_tasks(py::module& m);

void enqueue_task(PTaskBase_t task);


#endif /* SRC_TASKS_H_ */

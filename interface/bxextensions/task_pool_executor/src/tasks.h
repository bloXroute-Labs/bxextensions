#include <tpe/task_pool_executor.h>
#include "task/main_task_base.h"

#ifndef SRC_TASKS_H_
#define SRC_TASKS_H_

typedef task::pool::TaskPoolExecutor TaskPoolExecutor_t;

void bind_tasks(py::module& m);

void enqueue_task(PMainTaskBase_t task);


#endif /* SRC_TASKS_H_ */

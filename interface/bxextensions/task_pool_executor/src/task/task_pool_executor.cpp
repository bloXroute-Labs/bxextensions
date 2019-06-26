#include <memory>

#include "src/task/task_pool_executor.h"
#include <tpe/task_pool_executor.h>

typedef task::pool::TaskPoolExecutor TaskPoolExecutor_t;
typedef std::shared_ptr<TaskPoolExecutor_t> PTaskPoolExecutor_t;

void bind_tpe(py::module& m) {
    py::class_<
            TaskPoolExecutor_t,
            PTaskPoolExecutor_t>(m, "TaskPoolExecutor")
            .def(
                    py::init<>(),
                    "initializing"
            )
            .def("init", &TaskPoolExecutor_t::init)
            .def("size", &TaskPoolExecutor_t::size)
            .def("enqueue_task", &TaskPoolExecutor_t::enqueue_task);
}


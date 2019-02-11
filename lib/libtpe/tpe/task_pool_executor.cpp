#include "tpe/task_pool_executor.h"

#include <unistd.h>



namespace task {
namespace pool {

TaskPoolExecutor::TaskPoolExecutor():
    _is_initialized(false)
{
}

void TaskPoolExecutor::init() {
	if (_is_initialized) {
		return;
	} else {
		_is_initialized = true;
	}
	int cpu_count = std::thread::hardware_concurrency();
	size_t pool_size = std::max(cpu_count - 1, 1);
	_main_pool.init(
			pool_size,
			[&](std::shared_ptr<MainTaskBase> tsk) {
				tsk->execute(_sub_pool);
	});

	_sub_pool.init(
			pool_size, [](
			std::shared_ptr<SubTaskBase> tsk) {
				tsk->execute();
	});
}

void TaskPoolExecutor::enqueue_task(
    std::shared_ptr<task::MainTaskBase> task
)
{
	_main_pool.enqueue_task(task);
}

} // pool
} // task

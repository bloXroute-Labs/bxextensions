#include "tpe/task_pool_executor.h"

#include <unistd.h>



namespace task {
namespace pool {

TaskPoolExecutor::TaskPoolExecutor():
    _is_initialized(false)
{
}

void TaskPoolExecutor::init(size_t pool_size) {
	if (_is_initialized) {
		throw std::runtime_error("init was already called!"); // TODO : replace with proper exception here
	} else {
		_is_initialized = true;
	}
	if (pool_size > std::thread::hardware_concurrency()) {
		throw std::runtime_error("pool size cannot exceed the system cpu count!"); // TODO : replace with proper exception here
	}
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

size_t TaskPoolExecutor::try_init() {
	if (!_is_initialized) {
		int cpu_count = std::thread::hardware_concurrency();
		init(std::max(cpu_count - 1, 1));
	}
	return size();
}

void TaskPoolExecutor::enqueue_task(
    std::shared_ptr<task::MainTaskBase> task
)
{
	try_init(); // TODO : remove this once handled correctly in Python
	_main_pool.enqueue_task(std::move(task));
}

size_t TaskPoolExecutor::size(void) const {
	return _main_pool.size();
}

} // pool
} // task

#include <type_traits>
#include <utils/concurrency/thread_pool.h>

#include "tpe/task/task_base.h"

#ifndef TPE_TASK_TASK_THREAD_POOL_H_
#define TPE_TASK_TASK_THREAD_POOL_H_

namespace task {

template<class TTask>
using TThreadPool = utils::concurrency::ThreadPool<TTask>;

template <class TTask>
class TaskThreadPool : public TThreadPool<TTask> {
	typedef TThreadPool<TTask> TThreadPool_t;

	static_assert(
			std::is_base_of<TaskBase, TTask>::value,
			"task thread pool template must me TaskBase"
	);

public:
	TaskThreadPool(): TThreadPool_t(){}

	void enqueue_task(std::shared_ptr<TTask> task) {
		int pool_idx = this->get_available_queue();
		task->before_execution(pool_idx);
		TThreadPool_t::enqueue_item(task, pool_idx);
	}
};

}





#endif /* LIB_LIBTPE_TPE_TASK_TASK_THREAD_POOL_H_ */

#include "tpe/task/sub_task/sub_task_base.h"

#include <utils/exception/aggregated_exception.h>

namespace task {

SubTaskBase::SubTaskBase():
    TaskBase(),
    _mtx(),
    _condition() {

}

void SubTaskBase::execute() {
	std::lock_guard<std::mutex> lg(_mtx);
	try {
		_execute();
		after_execution();
	} catch (...) {
		utils::exception::AggregatedException e(
			  std::current_exception()
		);
		std::exception_ptr eptr = std::make_exception_ptr(e);
		after_execution(eptr);
	}
	_condition.notify_all();
}

void SubTaskBase::wait(void) {
	std::unique_lock<std::mutex> lock(_mtx);
	if (!is_completed()) {
		_condition.wait(lock);
	}
	_assert_execution();
}

}

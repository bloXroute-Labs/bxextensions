#include "tpe/task/sub_task/sub_task_base.h"

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
		after_execution(std::current_exception());
	}
	_condition.notify_all();
}

void SubTaskBase::wait(void) {
	std::unique_lock<std::mutex> lock(_mtx);
	if (!is_completed()) {
		_condition.wait(lock);
	}
}

}

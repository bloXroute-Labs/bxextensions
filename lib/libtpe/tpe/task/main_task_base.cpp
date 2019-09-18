#include <exception>
#include "tpe/task/main_task_base.h"

#include <utils/exception/aggregated_exception.h>

namespace task {

MainTaskBase::MainTaskBase(): TaskBase() {

}

void MainTaskBase::execute(SubPool_t& sub_pool) {
	try {
		_execute(sub_pool);
		after_execution();
	} catch (...) {
		utils::exception::AggregatedException e(
			  std::current_exception()
		);
		std::exception_ptr eptr = std::make_exception_ptr(e);
		after_execution(eptr);
	}
}

size_t MainTaskBase::get_task_byte_size() const {
    return sizeof(*this);
}

}

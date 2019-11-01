#include "tpe/task/exception/task_not_completed.h"

namespace task {
namespace exception {

TaskNotCompleted::TaskNotCompleted(unsigned long long task_id):
		ErrorBase_t(ERROR_TYPE),
		_task_id(task_id)
{
}

TaskNotCompleted::TaskNotCompleted():
		ErrorBase_t(ERROR_TYPE),
		_task_id(0)
{
}

const char* TaskNotCompleted::what(void) const noexcept {
    return utils::common::concatenate(
  		  "task ",
			  _task_id,
			  " did not complete!\n",
			  _get_backtrace()
	  ).c_str();
}

const std::string TaskNotCompleted::ERROR_TYPE = "TaskNotCompletedError";


} // exception
} // task

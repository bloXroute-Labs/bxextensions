#include "tpe/task/exception/task_not_executed.h"

namespace task {
namespace exception {

TaskNotExecuted::TaskNotExecuted(unsigned long long task_id):
		ErrorBase_t(ERROR_TYPE),
		_task_id(task_id)
{
}

TaskNotExecuted::TaskNotExecuted():
				ErrorBase_t(ERROR_TYPE),
				_task_id(0)
{
}

const char* TaskNotExecuted::what(void) const noexcept {
	  return utils::common::concatinate(
			  	  "task ",
				  _task_id,
				  " was never executed!\n",
				  _get_backtrace()
	  ).c_str();
}

const std::string TaskNotExecuted::ERROR_TYPE = "TaskNotExecutedError";

} // exception
} // task

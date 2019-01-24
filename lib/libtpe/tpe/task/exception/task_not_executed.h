#include <iostream>

#include "utils/common/string_helper.h"

#ifndef SRC_TASK_EXCEPTION_TASK_NOT_EXECUTED_H_
#define SRC_TASK_EXCEPTION_TASK_NOT_EXECUTED_H_

namespace task {
namespace exception {

class TaskNotExecuted : public std::exception {
public:
  TaskNotExecuted(unsigned long long task_id):
    std::exception(),
    _task_id(task_id) {

  }

  const char* what(void) const noexcept {
    return utils::common::concatinate(
	"task ", _task_id, " was never executed!"
	).c_str();
  }

private:
  unsigned long long _task_id;
};
} //task
} //exception



#endif /* SRC_TASK_EXCEPTION_TASK_NOT_EXECUTED_H_ */

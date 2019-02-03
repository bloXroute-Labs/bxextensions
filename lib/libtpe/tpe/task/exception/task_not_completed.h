#include <iostream>

#include <utils/common/string_helper.h>

#ifndef TPE_TASK_EXCEPTION_TASK_NOT_COMPLETED_H_
#define TPE_TASK_EXCEPTION_TASK_NOT_COMPLETED_H_

namespace task {
namespace exception {

class TaskNotCompleted : public std::exception {
public:
  TaskNotCompleted(unsigned long long task_id):
    std::exception(),
    _task_id(task_id) {

  }

  const char* what(void) const noexcept {
    return utils::common::concatinate(
	"task ", _task_id, " did not complete!"
	).c_str();
  }

private:
  unsigned long long _task_id;
};
} //task
} //exception

#endif /* TPE_TASK_EXCEPTION_TASK_NOT_COMPLETED_H_ */

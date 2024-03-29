#include <iostream>

#include <utils/common/string_helper.h>
#include <utils/exception/error_base.h>

#ifndef TPE_TASK_EXCEPTION_TASK_NOT_COMPLETED_H_
#define TPE_TASK_EXCEPTION_TASK_NOT_COMPLETED_H_

namespace task {
namespace exception {

typedef utils::exception::ErrorBase ErrorBase_t;

class TaskNotCompleted : public ErrorBase_t {
public:
  static const std::string ERROR_TYPE;

  TaskNotCompleted(unsigned long long task_id);
  TaskNotCompleted();

  const char* what(void) const noexcept;

private:
  unsigned long long _task_id;
};

} //task
} //exception

#endif /* TPE_TASK_EXCEPTION_TASK_NOT_COMPLETED_H_ */

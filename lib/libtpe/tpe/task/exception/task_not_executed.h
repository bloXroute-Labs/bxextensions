#include <iostream>

#include <utils/common/string_helper.h>
#include <utils/exception/error_base.h>

#ifndef TPE_TASK_EXCEPTION_TASK_NOT_EXECUTED_H_
#define TPE_TASK_EXCEPTION_TASK_NOT_EXECUTED_H_

namespace task {
namespace exception {

typedef utils::exception::ErrorBase ErrorBase_t;

class TaskNotExecuted : public ErrorBase_t {
public:
  static const std::string ERROR_TYPE;

  TaskNotExecuted(unsigned long long task_id);

  const char* what(void) const noexcept;

private:
  unsigned long long _task_id;
};

} //task
} //exception



#endif /* TPE_TASK_EXCEPTION_TASK_NOT_EXECUTED_H_ */

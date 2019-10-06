#include <iostream>


#ifndef TPE_TASK_BASE_H_
#define TPE_TASK_BASE_H_

namespace task {
class TaskBase {
public:

  virtual ~TaskBase() = default;

  unsigned long long get_id();

  void before_execution(int current_queue_idx);

  void after_execution(std::exception_ptr error = nullptr);

  bool is_completed();

  int current_queue_idx();

  void assert_execution();

protected:
  TaskBase();

  void _check_error();

  unsigned long long _task_id;
  volatile bool _is_completed;

private:
  std::exception_ptr _error;
  volatile int _current_queue_idx;
  volatile unsigned long long _last_executed_id;
  volatile bool _is_initialized;
  static unsigned long long _TASK_ID_CTR;
};
}

#endif // TPE_TASK_BASE_H_

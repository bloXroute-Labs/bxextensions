#include <iostream>


#ifndef TPE_TASK_BASE_H_
#define TPE_TASK_BASE_H_

namespace task {
class TaskBase {
public:

  virtual ~TaskBase() {}

  unsigned long long get_id(void);

  void before_execution(int current_queue_idx);

  void after_execution(std::exception_ptr error = nullptr);

  bool is_completed(void);

  int current_queue_idx(void);

protected:
  TaskBase();

  void _check_error(void);

  void _assert_execution(void);

  unsigned long long _task_id;
  volatile bool _is_completed;

private:
  std::exception_ptr _error;
  volatile int _current_queue_idx;
  unsigned long long _last_executed_id;
  bool _is_initialized;
  static unsigned long long _TASK_ID_CTR;
};
}

#endif // TPE_TASK_BASE_H_

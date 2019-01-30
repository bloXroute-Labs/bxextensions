#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>


#ifndef TPE_TASK_BASE_H_
#define TPE_TASK_BASE_H_

namespace task {
class TaskBase {
public:

  virtual ~TaskBase() {}

  unsigned long long get_id(void);

  void execute(void);

  bool is_completed(void);

  void wait(void);

  void added_to_queue(void);

  void reset(void);

protected:
  TaskBase();

  virtual void _execute(void) = 0;

  void _check_error(void);

  void _wait_if_needed(void);

  unsigned long long _task_id;
  volatile bool _is_completed;

private:
  std::exception_ptr _error;
  std::condition_variable _condition;
  std::mutex _mtx;
  volatile bool _added_to_queue;
  bool _is_initialized;
  static unsigned long long _TASK_ID_CTR;
};
}

#endif // TPE_TASK_BASE_H_

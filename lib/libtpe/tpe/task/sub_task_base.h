#include <thread>
#include <mutex>
#include <condition_variable>
#include "tpe/task/task_base.h"

#ifndef TPE_TASK_SUB_TASK_BASE_H_
#define TPE_TASK_SUB_TASK_BASE_H_

namespace task {

class SubTaskBase : public TaskBase {
public:
  SubTaskBase();

  void wait(void);
  void execute(void);

protected:
  virtual void _execute(void) = 0;

private:
  std::mutex _mtx;
  std::condition_variable _condition;
};

}



#endif /* TPE_TASK_SUB_TASK_BASE_H_ */

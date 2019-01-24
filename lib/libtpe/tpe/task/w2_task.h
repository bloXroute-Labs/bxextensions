#include "task_base.h"

#ifndef TPE_TASK_W2_TASK_H_
#define TPE_TASK_W2_TASK_H_

namespace task {

class W2Task : public TaskBase {
public:
  W2Task();

  void init(unsigned long long nw2);

protected:
  void _execute(void) override;

private:
  unsigned long long _nw2;

};

} // task

#endif /* TPE_TASK_W2_TASK_H_ */

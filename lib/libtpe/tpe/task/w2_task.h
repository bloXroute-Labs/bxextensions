#include "main_task_base.h"

#ifndef TPE_TASK_W2_TASK_H_
#define TPE_TASK_W2_TASK_H_

namespace task {

class W2Task : public MainTaskBase {
public:
  W2Task();

  void init(unsigned long long nw2);

protected:
  void _execute(SubPool_t& sub_pool) override;

private:
  unsigned long long _nw2;

};

} // task

#endif /* TPE_TASK_W2_TASK_H_ */

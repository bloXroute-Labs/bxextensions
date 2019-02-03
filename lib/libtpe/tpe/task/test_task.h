#include "tpe/task/main_task_base.h"

#ifndef TPE_TASK_TEST_TASK_H_
#define TPE_TASK_TEST_TASK_H_

namespace task {

class TestTask : public MainTaskBase {
public:
  TestTask();

  void init(unsigned long long t1);

  unsigned long long t1(void);
  unsigned long long t2(void);
  unsigned long long t3(void);

protected:
  void _execute(SubPool_t& sub_pool) override;

private:
  void _do_something(void);

  unsigned long long _t1, _t2, _t3;
};

}



#endif /* TPE_TASK_TEST_TASK_H_ */

#include "../../../libtpe/tpe/task/task_base.h"

#ifndef SRC_TASK_TEST_TASK_H_
#define SRC_TASK_TEST_TASK_H_

namespace task {

class TestTask : public TaskBase {
public:
  TestTask();

  void init(unsigned long long t1);

  unsigned long long t1(void);
  unsigned long long t2(void);
  unsigned long long t3(void);

protected:
  void _execute(void) override;

private:
  void _do_something(void);

  unsigned long long _t1, _t2, _t3;
};

}



#endif /* SRC_TASK_TEST_TASK_H_ */

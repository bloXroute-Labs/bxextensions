#include "tpe/task/main_task_base.h"

namespace task {

MainTaskBase::MainTaskBase(): TaskBase() {

}

void MainTaskBase::execute(SubPool_t& sub_pool) {
  try {
      _execute(sub_pool);
      after_execution();
  } catch (...) {
      after_execution(std::current_exception());
  }
}

}

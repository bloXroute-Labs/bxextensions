#include "tpe/task/sub_task_base.h"

namespace task {

SubTaskBase::SubTaskBase():
    TaskBase(),
    _mtx(),
    _condition() {

}

void SubTaskBase::execute() {
  try {
      _execute();
      after_execution();
  } catch (...) {
      after_execution(std::current_exception());
  }
  _condition.notify_all();
}

void SubTaskBase::wait(void) {
  std::unique_lock<std::mutex> lock(_mtx);
  _condition.wait(lock);
}

}

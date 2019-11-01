#include "tpe/task/exception/task_not_executed.h"
#include "tpe/task/task_base.h"
#include "tpe/task/exception/task_not_completed.h"

namespace task {

TaskBase::TaskBase():
    _is_completed(false),
    _is_initialized(false),
    _current_queue_idx(-1),
    _last_executed_id(0){
    _task_id = ++_TASK_ID_CTR;
}

unsigned long long TaskBase::get_id() {
    return _task_id;
}

bool TaskBase::is_completed() {
  return _is_completed;
}

void TaskBase::before_execution(int current_queue_idx) {
  if (_is_initialized) {
      assert_execution();
    _task_id = ++_TASK_ID_CTR;
    _is_completed = false;
  } else {
      _is_initialized = true;
  }
  _current_queue_idx = current_queue_idx;
}

void TaskBase::after_execution(
    std::exception_ptr error/* = nullptr*/) {
    _error = error;
    _last_executed_id = _task_id;
    _current_queue_idx = -1;
    _is_completed = true;
}

int TaskBase::current_queue_idx() {
  return _current_queue_idx;
}

void TaskBase :: _check_error() {
  if (_error != nullptr) {
      std::exception_ptr err = _error;
      _error = nullptr;
      std::rethrow_exception(err);
  }
}

void TaskBase::assert_execution() {
  if (_task_id != _last_executed_id) {
      throw exception::TaskNotExecuted(get_id());
  }
  if (!is_completed()) {
      throw exception::TaskNotCompleted(get_id());
  }
  _check_error();
}

unsigned long long TaskBase::_TASK_ID_CTR = 0;

}

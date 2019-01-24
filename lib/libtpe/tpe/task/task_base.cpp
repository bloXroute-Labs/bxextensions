#include "tpe/task/exception/task_not_executed.h"
#include "tpe/task/task_base.h"

#include "tpe/task/exception/task_not_executed.h"

namespace task {

TaskBase::TaskBase():
    _is_completed(false),
    _condition(),
    _mtx(),
    _added_to_queue(false),
    _is_initialized(false) {
    _task_id = _TASK_ID_CTR++;
}

unsigned long long TaskBase::get_id() {
    return _task_id;
}

bool TaskBase::is_completed() {
  return _is_completed;
}

void TaskBase::execute() {
  try {
      _execute();
  } catch (...) {
      _error = std::current_exception();
  }
  _is_completed = true;
  _condition.notify_all();
}

void TaskBase::wait() {
  std::unique_lock<std::mutex> lock(_mtx);
  if (!_added_to_queue) {
      throw exception::TaskNotExecuted(get_id());
  }
  while (!_is_completed) {
      _condition.wait(lock);
  }
}

void TaskBase::added_to_queue() {
  _added_to_queue = true;
}

void TaskBase :: _check_error() {
  if (_error != nullptr) {
      std::exception_ptr err = _error;
      _error = nullptr;
      std::rethrow_exception(err);
  }
}

void TaskBase::reset() {
  if (_is_initialized) {
    if (!is_completed()) {
	throw exception::TaskNotExecuted(get_id());
    }
    _check_error();
    _task_id = _TASK_ID_CTR++;
    _is_completed = false;
    _added_to_queue = false;
  } else {
      _is_initialized = true;
  }
}

void TaskBase::_wait_if_needed() {
  if (!is_completed()) {
      wait();
  }
  _check_error();
}

unsigned long long TaskBase::_TASK_ID_CTR = 0;

}

#include "../../../libtpe/tpe/task/test_task.h"

#include <chrono>


namespace task {

TestTask::TestTask():
    TaskBase(), _t1(0), _t2(0), _t3(0) {

}

void TestTask::init(unsigned long long t1) {
  _t1 = t1;
}

unsigned long long TestTask::t1() {
  return _t1;
}

unsigned long long TestTask::t2() {
  return _t2;
}

unsigned long long TestTask::t3() {
  return _t3;
}

void TestTask::_execute() {
  _t2 = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();
  _do_something();
  _t3 = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();
//  std::cout << "t1: " << _t1 << ", t2: " << _t2 <<
//      ", t3: " << _t3 << std::endl;
}

void TestTask::_do_something() {
  for (int i = 0 ; i < 1000000000 ; ++i) {
      volatile int j = i;
  }
}

}

#include "w2_task.h"

namespace task {

W2Task::W2Task() :
    TaskBase(), _nw2(0) {

}

void W2Task::init(unsigned long long nw2) {
  _nw2 = nw2;
  reset();
}

void W2Task::_execute(void) {
  for (unsigned long long i = 0 ;
      i < _nw2 ;
      ++i) {
      volatile unsigned long long j = i;
  }
}




} // task

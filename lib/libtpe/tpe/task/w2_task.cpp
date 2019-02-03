#include "w2_task.h"

namespace task {

W2Task::W2Task() :
    MainTaskBase(), _nw2(0) {

}

void W2Task::init(unsigned long long nw2) {
  _nw2 = nw2;
}

void W2Task::_execute(SubPool_t& sub_pool) {
  for (unsigned long long i = 0 ;
      i < _nw2 ;
      ++i) {
      volatile unsigned long long j = i;
  }
}




} // task

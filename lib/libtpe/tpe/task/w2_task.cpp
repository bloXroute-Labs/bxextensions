#include "w2_task.h"

namespace task {

W2Task::W2Task() :
    MainTaskBase(), _nw2(0) {

}

void W2Task::init(unsigned long long nw2) {
  _nw2 = nw2;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void W2Task::_execute(SubPool_t& sub_pool) {
    volatile unsigned long long j = 0;
    for (unsigned long long i = 0; i < _nw2; ++i) {
        j = i;
    }

    if (j == 0) {
        std::cout << "error in W2Task::_execute" << std::endl;
    }
}
#pragma GCC diagnostic pop

} // task

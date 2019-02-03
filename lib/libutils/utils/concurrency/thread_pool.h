#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include "utils/concurrency/queue_thread.h"


#ifndef UTILS_CONCURRENCY_THREAD_POOL_H_
#define UTILS_CONCURRENCY_THREAD_POOL_H_

namespace utils {
namespace concurrency {


template<typename TItem>
class ThreadPool {

  typedef std::unique_ptr<QueueThread<TItem>> PThread_t;

private:
  std::vector<PThread_t> _pool;
  volatile int _current_thread_idx;

public:
  void init(
      size_t pool_size,
      const std::function<
	void(std::shared_ptr<TItem>)>& item_dequeued_func
  )
  {
    for (int i = 0 ;
        i < pool_size ;
        ++i) {
	_pool.push_back(
	    PThread_t(
  	      new QueueThread<TItem>(item_dequeued_func)));
    }
  }

  void stop_requested(void) {
    std::for_each(_pool.begin(), _pool.end(),
		  [](PThread_t thread) {
      thread->stop_requested();
    });
  }

  int get_available_queue(void) {
    const int queue_idx =  _current_thread_idx;
    _current_thread_idx = ++_current_thread_idx % _pool.size();
    return queue_idx;
  }

  void enqueue_item(std::shared_ptr<TItem> pitem, int queue_idx = -1)
  {
    if(queue_idx < 0) {
	queue_idx = get_available_queue();
    }
    assert(queue_idx > 0 && queue_idx < _pool.size());
    _pool[queue_idx]->enqueue(pitem);
  }

  size_t size(void) {
    return _pool.size();
  }

};

} // concurrency
} // utils

#endif /* UTILS_CONCURRENCY_THREAD_POOL_H_ */

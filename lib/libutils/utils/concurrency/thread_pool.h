#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <functional>
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

public:

  ThreadPool() = default;
  ThreadPool(const ThreadPool<TItem>& other) = delete;

  void init(
      size_t pool_size,
      const std::function<
	  	  void(std::shared_ptr<TItem>)>& item_dequeued_func
  )
  {
    for (size_t i = 0 ;
        i < pool_size ;
        ++i) {
	_pool.push_back(
	    std::move(PThread_t(
  	      new QueueThread<TItem>(item_dequeued_func))));
    }
  }

  void stop_requested() {
	  for (auto& pthread: _pool) {
		  pthread->stop_requested();
	  }
  }

  int get_available_queue() {
    size_t best_queue_idx = 0, best_queue_size = 0;
    for (size_t idx = 0 ; idx < _pool.size() ; ++idx) {
    	PThread_t& thread = _pool[idx];
    	size_t queue_size = thread->queue_size();
		if (queue_size == 0) {
			return idx;
		} else if (queue_size < best_queue_size) {
			best_queue_size = queue_size;
			best_queue_idx = idx;
		}
    }
    return best_queue_idx;
  }

  size_t size() const {
	  return _pool.size();
  }

  void enqueue_item(
      std::shared_ptr<TItem> pitem,
      int queue_idx = -1
  )
  {
	  if (queue_idx < 0) {
		  queue_idx = get_available_queue();
	  }
	  assert(queue_idx >= 0 and (unsigned int) queue_idx < size());
	  _pool[queue_idx]->enqueue(pitem);
  }

};

} // concurrency
} // utils

#endif /* UTILS_CONCURRENCY_THREAD_POOL_H_ */

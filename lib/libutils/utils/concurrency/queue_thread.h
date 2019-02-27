#include <iostream>
#include <thread>
#include <memory>
#include <functional>

#include "safe_queue.h"

#ifndef UTILS_CONCURRENCY_QUEUE_THREAD_H_
#define UTILS_CONCURRENCY_QUEUE_THREAD_H_

namespace utils {
namespace concurrency {

template<typename TItem>
class QueueThread {
public:

  QueueThread(
      const std::function<void(
	  std::shared_ptr<TItem>)>& item_dequeued_func
      ): _item_dequeued_func(item_dequeued_func),
	 _stop_requested(false) {
    _queue_thread = std::unique_ptr<std::thread>(
	new std::thread([&](){
      _thread_loop();
    }));
  }

  QueueThread(const QueueThread<TItem>& other) = delete;

  virtual ~QueueThread() {
    stop_requested();
    _queue_thread->join();
  }

  void enqueue(std::shared_ptr<TItem> item) {
    _thread_queue.enqueue(item);
  }

  void stop_requested(void) {
    _stop_requested = true;
    _thread_queue.stop_requested();
  }

  size_t queue_size(void) const {
	  return _thread_queue.queue_size();
  }

private:

  void _thread_loop(void) {
    while (!_stop_requested) {
		std::shared_ptr<TItem> item = _thread_queue.dequeue();
		if (item != nullptr) {
			_item_dequeued_func(item);
		} else {
			break;
		}
    }
  }

  std::unique_ptr<std::thread> _queue_thread;
  std::function<void(std::shared_ptr<TItem>)> _item_dequeued_func;
  SafeQueue<TItem> _thread_queue;
  volatile bool _stop_requested;

};

} // concurrency
} // utils

#endif /* UTILS_CONCURRENCY_QUEUE_THREAD_H_ */

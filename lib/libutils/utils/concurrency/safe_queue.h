#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

#ifndef UTILS_CONCURRENCY_SAFE_QUEUE_H_
#define UTILS_CONCURRENCY_SAFE_QUEUE_H_

namespace utils {
namespace concurrency {

template <typename TItem>
class SafeQueue {
public:
  SafeQueue(): _queue(), _mtx(), _condition(),
  _stop_requested(false) {

  }

  virtual ~SafeQueue() {

  }

  void enqueue(std::shared_ptr<TItem> item) {
		std::lock_guard<std::mutex> lock(_mtx);
		_queue.push(item);
		_condition.notify_one();
  }

  std::shared_ptr<TItem> dequeue() {
		std::unique_lock<std::mutex> lock(_mtx);
		while (_queue.empty() && !_stop_requested) {
		  _condition.wait(lock);
		}
		std::shared_ptr<TItem> val;
		if (_queue.empty()) {
			val = nullptr;
		}
		else {
			val = std::move(_queue.front());
			_queue.pop();
		}
		return val;
  }

  void stop_requested() {
		std::lock_guard<std::mutex> lock(_mtx);
		_stop_requested = true;
		_condition.notify_all();
  }

  size_t queue_size() const {
		return _queue.size();
  }

private:
  std::queue<std::shared_ptr<TItem>> _queue;
  mutable std::mutex _mtx;
  std::condition_variable _condition;
  volatile bool _stop_requested;
};

} // concurrency
} // utils


#endif /* UTILS_CONCURRENCY_SAFE_QUEUE_H_ */

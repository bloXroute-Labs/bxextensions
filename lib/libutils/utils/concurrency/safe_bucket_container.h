#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "utils/common/bucket.h"

#ifndef UTILS_CONCURRENCY_SAFE_BUCKET_CONTAINER_H_
#define UTILS_CONCURRENCY_SAFE_BUCKET_CONTAINER_H_

namespace utils {
namespace concurrency {

template <
	class T,
	class THash = std::hash<T>,
	class TPred = std::equal_to<T>,
    class TAllocator = std::allocator<T>
>
class SafeBucketContainer {
public:

	typedef common::Bucket<T, THash, TPred, TAllocator> Bucket_t;
	typedef std::vector<Bucket_t> Buckets_t;
	typedef std::queue<T> ItemsToAdd_t;
	typedef std::queue<std::pair<T, size_t>> ItemsToRemove_t;
	typedef std::function<bool(const T&)> ItemRemovePredicate_t;

    SafeBucketContainer(
			size_t bucket_capacity, uint32_t bucket_count, TAllocator allocator = TAllocator()
	):
		_reader_count(0),
		_bucket_capacity(bucket_capacity),
		_bucket_count(bucket_count),
		_mtx(),
		_current_bucket_idx(0),
		_stop_requested(false)
	{
		for (size_t idx = 0 ; idx < _bucket_count ; ++idx) {
			_buckets.push_back(
					std::move(Bucket_t(bucket_capacity, idx, allocator))
			);
		}
	}

	virtual ~SafeBucketContainer() = default;

	const Bucket_t& operator[](uint32_t idx) const {
		return _buckets.at(idx);
	}

	bool is_reading() const {
		return _reader_count > 0;
	}

	bool is_write_pending() const {
		return !_items_to_add.empty() || !_items_to_remove.empty();
	}

	void acquire_read() {
		std::unique_lock<std::mutex> lock(_mtx);
		if (is_write_pending()) {
			_write_condition.wait(
					lock,
					[&](){return !is_write_pending() || _stop_requested;}
			);
		}
		++_reader_count;
	}

	void release_read() {
		std::lock_guard<std::mutex> lock(_mtx);
		--_reader_count;
		if (is_reading()) {
			return;
		}
		while(!_items_to_add.empty()) {
			const T& item = _items_to_add.front();
			_insert_to_container(item);
            _items_to_add.pop();
        }
		while(!_items_to_remove.empty()) {
			const std::pair<T, uint32_t>& pair = _items_to_remove.front();
			_remove_from_container(pair);
            _items_to_remove.pop();
        }
		_write_condition.notify_all();
	}

	void insert(const T& item) {
		std::lock_guard<std::mutex> lock(_mtx);
		if (is_reading()) {
			_items_to_add.push(item);
		} else {
			_insert_to_container(item);
		}
	}

	void erase(const T& item) {
		std::lock_guard<std::mutex> lock(_mtx);
		if (is_reading()) {
			uint32_t idx;
			if(_find_bucket(item, idx)) {
				_items_to_remove.push(
						std::make_pair(item, idx)
				);
			}
		} else {
			_remove_from_container(item);
		}
	}

	void clear() {
        std::lock_guard<std::mutex> lock(_mtx);
        for (Bucket_t &bucket: _buckets) {
            if (is_reading()) {
                for (const T &item: bucket) {
                    _items_to_remove.push(
                        std::make_pair(item, bucket.bucket_index())
                    );
                }
            } else {
                bucket.clear();
            }
        }
    }

	void erase(const ItemRemovePredicate_t& predicate) {
		std::lock_guard<std::mutex> lock(_mtx);
		for (Bucket_t& bucket: _buckets) {
			for(const T& item: bucket) {
				if (predicate(item)) {
					if (is_reading()) {
						_items_to_remove.push(
								std::make_pair(item, bucket.bucket_index())
						);
					} else {
						_remove_from_container(
								std::make_pair(item, bucket.bucket_index())
						);
					}
				}
			}
		}
	}

	size_t size() const {
		size_t total = 0;
		for (const Bucket_t& bucket: _buckets) {
			total += bucket.size();
		}
		return total;
	}

	uint32_t bucket_count() const {
		return _bucket_count;
	}

	void stop_requested() {
		std::lock_guard<std::mutex> lock(_mtx);
		_stop_requested = true;
		_write_condition.notify_all();
	}

	typename Buckets_t::const_iterator begin() const {
		return _buckets.begin();
	}

	typename Buckets_t::const_iterator end() const {
		return _buckets.end();
	}


private:

	void _insert_to_container(const T& item) {
		if(_buckets[_current_bucket_idx].is_full()) {
			_current_bucket_idx = (_current_bucket_idx + 1) % _bucket_count;
		}
		Bucket_t& bucket = _buckets[_current_bucket_idx];
		bucket.insert(item);
	}

	void _remove_from_container(const T& item) {
		for (Bucket_t& bucket: _buckets) {
			if (bucket.erase(item)) {
				break;
			}
		}
	}

	void _remove_from_container(const std::pair<T, uint32_t>& pair) {
		_buckets[pair.second].erase(pair.first);
	}

	bool _find_bucket(const T& val, uint32_t& idx) {
		for (Bucket_t& bucket: _buckets) {
			if (bucket.contains(val)) {
				idx = bucket.bucket_index();
				return true;
			}
		}
		return false;
	}

	Buckets_t _buckets;
	ItemsToAdd_t _items_to_add;
	ItemsToRemove_t _items_to_remove;
	volatile uint32_t _reader_count;
	volatile uint32_t _current_bucket_idx;
	volatile bool _stop_requested;
	const size_t _bucket_capacity;
	const uint32_t _bucket_count;
	mutable std::mutex _mtx;
	std::condition_variable _write_condition;

};

} // concurrency
} // utils

#endif /* UTILS_CONCURRENCY_SAFE_BUCKET_CONTAINER_H_ */

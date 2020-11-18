#include <iostream>
#include <unordered_set>

#ifndef UTILS_COMMON_BUCKET_H_
#define UTILS_COMMON_BUCKET_H_

namespace utils {
namespace common {

template <
	class T,
	class THash = std::hash<T>,
	class TPred = std::equal_to<T>,
    class TAllocator = std::allocator<T>
>
class Bucket {
public:
	typedef std::unordered_set<T, THash, TPred, TAllocator> Set_t;
	typedef typename Set_t::iterator iterator;
	typedef typename Set_t::const_iterator const_iterator;

	Bucket(size_t capacity, uint32_t bucket_index, TAllocator allocator = std::allocator<T>()):
		_set(allocator),
		_bucket_index(bucket_index),
		_capacity(capacity)
	{
		_set.reserve(capacity);
	}

	Bucket(Bucket&& other) noexcept {
		_set = other._set;
		_bucket_index = other._bucket_index;
		_capacity = other._capacity;
	}

	Bucket& operator =(Bucket&& rhs) noexcept {
		_set = std::move(rhs._set);
		_bucket_index = rhs._bucket_index;
		_capacity = rhs._capacity;
		return *this;
	}

	iterator begin() {
		return _set.begin();
	}

	const_iterator begin() const {
		return _set.begin();
	}

	iterator end() {
		return _set.end();
	}
	const_iterator end() const {
		return _set.end();
	}

	uint32_t bucket_index() const {
		return _bucket_index;
	}

	size_t size() const {
		return _set.size();
	}

	bool is_full() const {
		return size() >= _capacity;
	}

	bool empty() const {
		return _set.empty();
	}

	std::pair<iterator, bool> insert(const T& val) {
		return _set.insert(val);
	}

	bool erase(const T& val) {
		return _set.erase(val) > 0;
	}

	bool contains(const T& val) const {
		return _set.find(val) != _set.end();
	}

	void clear() {
	    _set.clear();
	}

private:
	Set_t _set;
	uint32_t _bucket_index;
	size_t _capacity;
};

} // common
} // utils

#endif /* UTILS_COMMON_BUCKET_H_ */

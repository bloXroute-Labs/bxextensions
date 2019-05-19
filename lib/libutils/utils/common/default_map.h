#include <iostream>
#include <unordered_map>
#include <functional>

#include "utils/exception/key_error.h"

#ifndef UTILS_COMMON_DEFAULT_MAP_H_
#define UTILS_COMMON_DEFAULT_MAP_H_

namespace utils {
namespace common {

template<class TValue>
struct default_value_factory {
	TValue operator()() {
		TValue val;
		return val;
	}
};


template <
	class TKey,
	class TValue,
	class THash = std::hash<TKey>,
	class TPred = std::equal_to<TKey>,
	class ValueFactory = default_value_factory<TValue>
>
class DefaultMap {
public:
	typedef std::unordered_map<TKey, TValue, THash, TPred> Map_t;



	typename Map_t::iterator begin() {
		return _map.begin();
	}

	typename Map_t::iterator end() {
		return _map.end();
	}

	typename Map_t::iterator find(const TKey& key) {
		return _map.find(key);
	}

	typename Map_t::const_iterator begin() const {
		return _map.begin();
	}

	typename Map_t::const_iterator end() const {
		return _map.end();
	}

	typename Map_t::const_iterator find(const TKey& key) const {
		return _map.find(key);
	}

	TValue& operator[](const TKey& key) {
		auto iter = _map.find(key);
		if(iter == end()) {
			_map[key] = _value_factory();
		}
		return _map[key];
	}

	const TValue& operator[](const TKey& key) const {
		typename Map_t::const_iterator iter =  find(key);
		if (iter == end()) {
			throw exception::KeyError();
		}
		return iter->second;
	}

	size_t size(void) const {
		return _map.size();
	}

	void erase(const TKey& key) {
		_map.erase(key);
	}

	void clear(void) {
		_map.clear();
	}

private:
	Map_t _map;
	ValueFactory _value_factory;
};

} // common
} // utils

#endif /* LIB_LIBUTILS_UTILS_COMMON_DEFAULT_MAP_H_ */

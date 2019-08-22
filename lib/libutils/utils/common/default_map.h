#include <iostream>
#include <unordered_map>
#include <functional>

#include "utils/exception/key_error.h"

#ifndef UTILS_COMMON_DEFAULT_MAP_H_
#define UTILS_COMMON_DEFAULT_MAP_H_

namespace utils {
namespace common {

template<class TValue>
struct default_item_factory {

	TValue operator ()() {
		return std::move(TValue());
	}
};

template<class TValue>
using DefaultItemFactory_t = default_item_factory<TValue>;

template <
	class TKey,
	class TValue,
	class THash = std::hash<TKey>,
	class TPred = std::equal_to<TKey>,
    class TAllocator = std::allocator<std::pair<const TKey, TValue>>,
	class ItemFactory = default_item_factory<TValue>
>
class DefaultMap {
public:
	typedef std::unordered_map<TKey, TValue, THash, TPred, TAllocator> Map_t;
    using value_type = typename Map_t::value_type;

	explicit DefaultMap(
            TAllocator allocator = std::allocator<std::pair<const TKey, TValue>>(),
            ItemFactory item_factory = DefaultItemFactory_t<TValue>()
	): _item_factory(item_factory), _map(allocator)
	{
	}

	virtual ~DefaultMap() = default;

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

	virtual TValue& operator[](const TKey& key) {
		auto iter = _map.find(key);
		if(iter == end()) {
			_map[key] = std::move(_item_factory());
		}
		return _map[key];
	}

	TValue& at(const TKey& key) {
        return _map.at(key);
	}

	const TValue& operator[](const TKey& key) const {
		typename Map_t::const_iterator iter =  find(key);
		if (iter == end()) {
			throw exception::KeyError();
		}
		return iter->second;
	}

	size_t size() const {
		return _map.size();
	}

	virtual void erase(const TKey& key) {
		_map.erase(key);
	}

	virtual void clear() {
		_map.clear();
	}

	TAllocator get_allocator() const {
        return _map.get_allocator();
	}

protected:
	ItemFactory _item_factory;
	Map_t _map;
};

} // common
} // utils

#endif /* LIB_LIBUTILS_UTILS_COMMON_DEFAULT_MAP_H_ */

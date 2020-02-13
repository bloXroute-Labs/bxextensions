#include <unordered_map>
#include <list>
#include <utility>

#ifndef UTILS_COMMON_ORDERED_MAP_H_
#define UTILS_COMMON_ORDERED_MAP_H_

namespace utils {
namespace common {

template <
        class TKey,
        class TValue,
        class THash = std::hash<TKey>,
        class TPred = std::equal_to<TKey>,
        class Allocator_t = std::allocator<std::pair<TKey, TValue>>
>
class OrderedMap {
public:
    using TItem = std::pair<TKey, TValue>;
    using iterator = typename std::list<TItem, Allocator_t>::iterator;
    using const_iterator = typename std::list<TItem>::const_iterator;
    using MapAllocator_t = typename std::allocator_traits<Allocator_t>::template rebind_alloc<std::pair<const TKey, iterator>>;

    OrderedMap():
        _list(Allocator_t()),
        _map(MapAllocator_t(_list.get_allocator()))
    {
    }

    explicit OrderedMap(const Allocator_t& allocator):
        _list(allocator),
        _map(MapAllocator_t(_list.get_allocator()))
    {
    }

    std::pair<iterator, bool> insert(const TItem& item) {
        auto iter = find(item.first);
        if (iter == end()) {
            _list.push_back(item);
            auto pair = _map.insert(std::make_pair(
                    item.first,
                    std::prev(std::end(_list)))
            );
            return std::make_pair(pair.first->second, pair.second);
        } else {
            return std::make_pair(iter, false);
        }
    }

    std::pair<iterator, bool> emplace(const TKey& key, const TValue& value) {
        auto iter = find(key);
        if (iter == end()) {
            _list.emplace_back(std::move(key), std::move(value));
            auto pair = _map.emplace(key, std::prev(std::end(_list)));
            return std::make_pair(pair.first->second, pair.second);
        } else {
            return std::make_pair(iter, false);
        }
    }

    std::pair<iterator, bool> emplace(const TKey& key, TValue&& value) {
        auto iter = find(key);
        if (iter == end()) {
            _list.emplace_back(std::move(key), std::move(value));
            auto pair = _map.emplace(key, std::prev(std::end(_list)));
            return std::make_pair(pair.first->second, pair.second);
        } else {
            return std::make_pair(iter, false);
        }
    }

    iterator begin() {
        return std::begin(_list);
    }

    iterator end() {
        return std::end(_list);
    }

    iterator find(const TKey& key) {
        auto iter = _map.find(key);
        if (iter == _map.end()) {
            return end();
        } else {
            return iter->second;
        }
    }

    const_iterator begin() const {
        return std::begin(_list);
    }

    const_iterator end() const {
        return std::end(_list);
    }

    const_iterator find(const TKey& key) const {
        auto iter = _map.find(key);
        if (iter == _map.end()) {
            return end();
        } else {
            return iter->second;
        }
    }

    size_t erase(const TKey& key) {
        auto iter = find(key);
        if (iter != end()) {
            const size_t result = _map.erase(key);
            _list.erase(iter);
            return result;
        }
        return 0;
    }

    TValue& operator [](const TKey& key) {
        return _map[key]->second;
    }

    size_t size() const {
        return _list.size();
    }

    void clear() {
        _map.clear();
        _list.clear();
    }

    Allocator_t get_allocator() const {
        return _list.get_allocator();
    }


private:
    std::list<TItem, Allocator_t> _list;
    std::unordered_map<TKey, iterator, THash, TPred, MapAllocator_t> _map;
};

} // common
} // utils

#endif //UTILS_COMMON_ORDERED_MAP_H_

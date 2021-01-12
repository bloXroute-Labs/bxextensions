#include <iostream>
#include <unordered_map>
#include <memory>

#include "utils/exception/key_error.h"

#ifndef UTILS_COMMON_MAP_WRAPPER_H_
#define UTILS_COMMON_MAP_WRAPPER_H_

namespace utils {
namespace common {

template <class TValue>
struct AbstractValueTracker {

    virtual ~AbstractValueTracker() = default;

    virtual void on_value_added(const TValue& val) = 0;

    virtual void on_value_removed(TValue&& val) = 0;

    virtual void on_container_cleared() = 0;

    virtual size_t total_bytes_allocated() const  = 0;

};

template <class TValue>
struct DefaultValueTracker: public AbstractValueTracker<TValue> {

    DefaultValueTracker(): _total_bytes_allocated(0) {}

    DefaultValueTracker(const DefaultValueTracker&) = default;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void on_value_added(const TValue& val) override {
        _total_bytes_allocated += sizeof(TValue);
    }
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void on_value_removed(TValue&& val) override {
        size_t byte_size = sizeof(TValue);
        if (_total_bytes_allocated >= byte_size) {
            _total_bytes_allocated -= byte_size;
        }
    }
#pragma GCC diagnostic pop

    size_t total_bytes_allocated() const override {
        return _total_bytes_allocated;
    }

    void on_container_cleared() override {
        _total_bytes_allocated = 0;
    }

private:
    size_t _total_bytes_allocated;

};

template <
        class TKey,
        class TValue,
        class THash = std::hash<TKey>,
        class TPred = std::equal_to<TKey>,
        class TAllocator = std::allocator<std::pair<const TKey, TValue>>
>
class MapWrapper {
public:
    typedef std::unordered_map<TKey, TValue, THash, TPred, TAllocator> Map_t;
    typedef AbstractValueTracker<TValue> AbstractValueTracker_t;
    typedef std::unique_ptr<AbstractValueTracker_t> PAbstractValueTracker_t;
    typedef DefaultValueTracker<TValue> DefaultValueTracker_t;
    using value_type = typename Map_t::value_type;

    explicit MapWrapper(
            TAllocator allocator = TAllocator()
    ): _map(allocator)
    {
        _tracker = std::make_unique<DefaultValueTracker_t>(DefaultValueTracker_t());
    }

    template <class TValueTracker>
    MapWrapper(
            const TValueTracker& tracker,
            TAllocator allocator = TAllocator()
    ): _map(allocator)
    {
        _tracker = std::make_unique<TValueTracker>(tracker);
    }

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
        return _map[key];
    }

    TValue& at(const TKey& key) {
        return _map.at(key);
    }

    const TValue& at(const TKey& key) const {
        return _map.at(key);
    }

    const TValue& operator[](const TKey& key) const {
        typename Map_t::const_iterator iter =  find(key);
        if (iter == end()) {
            throw exception::KeyError();
        }
        return iter->second;
    }

    std::pair<typename Map_t::iterator, bool> emplace(const TKey& key, TValue&& value){
        _tracker->on_value_added(value);
        return _map.emplace(key, std::move(value));
    }

    std::pair<typename Map_t::iterator, bool> emplace(const TKey& key, const TValue& value) {
        _tracker->on_value_added(value);
        return _map.emplace(key, value);
    }

    size_t size() const {
        return _map.size();
    }

    void erase(const TKey& key) {
        auto iter = _map.find(key);
        if (iter != _map.end()) {
            TValue temp_value(iter->second);
            _map.erase(iter);
            _tracker->on_value_removed(std::move(temp_value));
        }
    }

    typename Map_t::iterator erase(typename Map_t::const_iterator iter) {
        if (iter != _map.end()) {
            TValue temp_value(iter->second);
            auto next_iter = _map.erase(iter);
            _tracker->on_value_removed(std::move(temp_value));
            return next_iter;
        }
        return _map.erase(iter);
    }

    void clear() {
        _map.clear();
        _tracker->on_container_cleared();
    }

    TAllocator get_allocator() const {
        return _map.get_allocator();
    }

    const AbstractValueTracker<TValue>& get_value_tracker() const {
        return *_tracker;
    }

private:
    Map_t _map;
    PAbstractValueTracker_t _tracker;
};

} // common
} // utils

#endif /* UTILS_COMMON_MAP_WRAPPER_H_ */

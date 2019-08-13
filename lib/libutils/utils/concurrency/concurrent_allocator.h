#include <iostream>
#include <memory>
#include <queue>
#include <mutex>
#include <vector>

#include "utils/concurrency/memory_allocation_thread.h"

#ifndef UTILS_CONCURRENCY_CONCURRENT_ALLOCATOR_H_
#define UTILS_CONCURRENCY_CONCURRENT_ALLOCATOR_H_

#define MAX_ALLOCATION_POINTER_COUNT 10
#define MAX_COUNT_PER_ALLOCATION 10

namespace utils {
namespace concurrency {

template <class T>
class ConcurrentAllocator {
public:
    using base_allocator = std::allocator<T>;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;

    typedef std::queue<std::pair<pointer, size_type>> DeallocationQueue_t;

    const size_t max_allocation_pointer_count;
    const size_t max_count_per_allocation;

    inline explicit ConcurrentAllocator(
            size_t max_allocation_pointer_count = MAX_ALLOCATION_POINTER_COUNT,
            size_t max_count_per_allocation = MAX_COUNT_PER_ALLOCATION,
            MemoryAllocationThread* executor = nullptr
    ):
        _executor(executor),
        max_allocation_pointer_count(max_allocation_pointer_count),
        max_count_per_allocation(max_count_per_allocation),
        _idx(0),
        _allocation_queues(max_count_per_allocation)
    {
        if (executor != nullptr) {
            register_executor(executor);
        }
    };
    inline ConcurrentAllocator(const ConcurrentAllocator& other):
        _executor(other._executor),
        max_allocation_pointer_count(other.max_allocation_pointer_count),
        max_count_per_allocation(other.max_count_per_allocation),
        _allocation_queues(max_count_per_allocation)

    {
        if (_executor != nullptr) {
            register_executor(_executor);
        }
    };

    template <class U>
    inline ConcurrentAllocator(const ConcurrentAllocator<U>& other):
        _executor(other.executor()),
        max_allocation_pointer_count(other.max_allocation_pointer_count),
        max_count_per_allocation(other.max_count_per_allocation),
        _allocation_queues(other.max_count_per_allocation)

    {
        if (_executor != nullptr) {
            register_executor(_executor);
        }
    }

    inline ~ConcurrentAllocator() {
        if (_executor != nullptr) {
            _executor->remove(_idx);
        }
        { // lock scope
            std::lock_guard<std::mutex> lock(_mtx);
            _drain_deallocation_queue(_deallocation_queue);
            for (auto& queue: _allocation_queues) {
                while (!queue.empty()) {
                    pointer p = queue.front();
                    ::operator delete(p);
                    queue.pop();
                }
            }
            _allocation_queues.clear();
            _executor = nullptr;
        }
    }

    inline pointer address(reference r) {
        return &r;
    }
    inline const_pointer address(const_reference r) {
        return &r;
    }


    inline pointer allocate(size_type n, const_pointer hint = 0) {
        pointer p;
        bool should_notify = false;
        { // lock scope
            std::lock_guard<std::mutex> lock(_mtx);
            if (n <= max_count_per_allocation && !_allocation_queues.at(n - 1).empty()) {
                p = _allocation_queues.at(n - 1).front();
                _allocation_queues.at(n - 1).pop();
                should_notify = true;
            } else {
                if (n <= max_count_per_allocation && _allocation_queues.at(n - 1).empty()) {
                    should_notify = true;
                }
                p = reinterpret_cast<pointer>(::operator new(n * sizeof(T)));
            }
        }
        if (should_notify) {
            _notify();
        }
        return p;
    }

    inline void deallocate(pointer p, size_type n) {
        { // lock scope
            std::lock_guard<std::mutex> lock(_mtx);
            if (_deallocation_queue.size() > max_allocation_pointer_count) {
                _drain_deallocation_queue(_deallocation_queue);
            }
            _deallocation_queue.push(std::make_pair(p, n));
        }
        _notify();
    }

    template <class U>
    struct rebind {
        typedef ConcurrentAllocator<U> other;
    };

    inline size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    inline void construct(pointer p, const T& t) {
        new(p) T(t);
    }
    inline void destroy(pointer p) {
        p->~T();
    }

    inline bool operator ==(const ConcurrentAllocator&) {
        return true;
    }
    inline bool operator !=(const ConcurrentAllocator& other) {
        return !operator==(other);
    }

    void register_executor(MemoryAllocationThread* other_executor) {
        _idx = other_executor->add(
                [&]() {
                    _check_allocations();
                }
        );
        _executor = other_executor;
    }

    MemoryAllocationThread * const executor() const {
        return _executor;
    }

private:
    void _check_allocations() {
        DeallocationQueue_t deallocation_queue_copy;
        { // lock scope
            std::unique_lock<std::mutex> lock(_mtx);
            _deallocation_queue.swap(deallocation_queue_copy);
            for (size_t i = 0 ; i < max_count_per_allocation ; ++i) {
                auto& allocation_queue = _allocation_queues.at(i);
                for (size_t j = allocation_queue.size() ; j < max_allocation_pointer_count ; ++j) {
                    lock.unlock(); // release the lock while allocating memory
                    auto p = reinterpret_cast<pointer>(::operator new((i + 1) * sizeof(T)));
                    lock.lock();
                    allocation_queue.push(p);
                }
            }
        }
        _drain_deallocation_queue(deallocation_queue_copy);
    }

    void _drain_deallocation_queue(DeallocationQueue_t& deallocation_queue) {
        while (!deallocation_queue.empty()) {
            auto pair = deallocation_queue.front();
            ::operator delete(pair.first);
            deallocation_queue.pop();
        }
    }

    void _notify() {
        if (_executor != nullptr) {
            _executor->condition().notify_one();
        }
    }

    std::vector<std::queue<pointer>> _allocation_queues;
    DeallocationQueue_t _deallocation_queue;
    std::mutex _mtx;
    uint32_t _idx;
    MemoryAllocationThread* _executor;
};

} // concurrency
} // utils

#endif //UTILS_CONCURRENCY_CONCURRENT_ALLOCATOR_H_

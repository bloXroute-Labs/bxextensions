#include <iostream>
#include <memory>

#ifndef UTILS_COMMON_TRACKED_ALLOCATOR_H_
#define UTILS_COMMON_TRACKED_ALLOCATOR_H_

namespace utils {
namespace common {

template <class T>
class TrackedAllocator {
public:
    using base_allocator = std::allocator<T>;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;

    TrackedAllocator(): _total_bytes_allocated(0)
    {
    }

    TrackedAllocator(const TrackedAllocator& other):
        _total_bytes_allocated(other.total_bytes_allocated())
    {
    }

    template <class U>
    explicit TrackedAllocator(const TrackedAllocator<U>& other):
            _total_bytes_allocated(other.total_bytes_allocated())
    {
    }

    inline pointer address(reference r) {
        return &r;
    }
    inline const_pointer address(const_reference r) {
        return &r;
    }

    size_t total_bytes_allocated() const {
        return _total_bytes_allocated;
    }


    inline pointer allocate(size_type n, const_pointer hint = 0) {
        size_type p_size = n * sizeof(T);
        auto p = reinterpret_cast<pointer>(::operator new(p_size));
        _total_bytes_allocated += p_size;
        return p;
    }

    inline void deallocate(pointer p, size_type n) {
        ::operator delete(p);
        _total_bytes_allocated -= (n * sizeof(T));
    }

    template <class U>
    struct rebind {
        typedef TrackedAllocator<U> other;
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

    inline bool operator ==(const TrackedAllocator&) {
        return true;
    }
    inline bool operator !=(const TrackedAllocator& other) {
        return !operator==(other);
    }

private:

    volatile size_t _total_bytes_allocated;
};

} // common
} // utils

#endif //UTILS_COMMON_TRACKED_ALLOCATOR_H_

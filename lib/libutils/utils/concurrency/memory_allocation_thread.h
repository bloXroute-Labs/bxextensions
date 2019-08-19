#include <iostream>
#include <condition_variable>
#include <functional>
#include <utility>
#include <unordered_map>
#include <thread>
#include <memory>
#include <queue>
#include <vector>

#ifndef UTILS_CONCURRENCY_MEMORY_ALLOCATION_THREAD_H_
#define UTILS_CONCURRENCY_MEMORY_ALLOCATION_THREAD_H_

namespace utils {
namespace concurrency {

class MemoryAllocationThread {
public:

    MemoryAllocationThread();

    ~MemoryAllocationThread();

    uint32_t add(std::function<void(void)> callback);

    void remove(uint32_t idx);

    void notify(uint32_t idx);

private:

    void _thread_loop();

    std::condition_variable _condition;
    std::mutex _mtx;
    volatile bool _stop_requested;
    uint32_t _last_idx;
    std::unordered_map<uint32_t, std::function<void(void)>> _callbacks;
    std::unique_ptr<std::thread> _thread;
    std::queue<uint32_t> _events;
};

} // concurrency
} // utils

#endif //UTILS_CONCURRENCY_MEMORY_ALLOCATION_THREAD_H_

#include <iostream>
#include <condition_variable>
#include <functional>
#include <utility>
#include <unordered_map>
#include <thread>
#include <memory>

#ifndef UTILS_CONCURRENCY_MEMORY_ALLOCATION_THREAD_H_
#define UTILS_CONCURRENCY_MEMORY_ALLOCATION_THREAD_H_

#define ALLOCATION_LOOP_SLEEP_MICROSECONDS 1000

namespace utils {
namespace concurrency {

class MemoryAllocationThread {
public:

    MemoryAllocationThread(int64_t thread_loop_sleep_microseconds = ALLOCATION_LOOP_SLEEP_MICROSECONDS);

    ~MemoryAllocationThread();

    uint32_t add(std::function<void(void)> callback);

    void remove(uint32_t idx);

    std::condition_variable& condition();

private:

    void _thread_loop();

    std::condition_variable _condition;
    std::mutex _mtx;
    volatile bool _stop_requested;
    uint32_t _last_idx;
    std::unordered_map<uint32_t, std::function<void(void)>> _callbacks;
    std::unique_ptr<std::thread> _thread;
    const int64_t _thread_loop_sleep_microseconds;
};

} // concurrency
} // utils

#endif //UTILS_CONCURRENCY_MEMORY_ALLOCATION_THREAD_H_

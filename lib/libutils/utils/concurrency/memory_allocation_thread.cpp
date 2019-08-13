#include "utils/concurrency/memory_allocation_thread.h"
#include <chrono>

namespace utils {
namespace concurrency {

MemoryAllocationThread::MemoryAllocationThread(int64_t thread_loop_sleep_microseconds/* = ALLOCATION_LOOP_SLEEP_MICROSECONDS*/):
    _stop_requested(false),
    _last_idx(0),
    _thread_loop_sleep_microseconds(thread_loop_sleep_microseconds)
{
    _thread = std::unique_ptr<std::thread>(new std::thread([&]() {_thread_loop();}));
}


MemoryAllocationThread::~MemoryAllocationThread() {
    _stop_requested = true;
    _condition.notify_all();
    _thread->join();
}

uint32_t MemoryAllocationThread::add(std::function<void(void)> callback) {
    std::lock_guard<std::mutex> lock(_mtx);
    uint32_t idx = _last_idx;
    ++_last_idx;
    _callbacks.emplace(idx, std::move(callback));
    return idx;
}

void MemoryAllocationThread::remove(uint32_t idx) {
    std::lock_guard<std::mutex> lock(_mtx);
    _callbacks.erase(idx);
}

std::condition_variable& MemoryAllocationThread::condition() {
    return _condition;
}

void MemoryAllocationThread::_thread_loop() {
    std::unique_lock<std::mutex> lock(_mtx);
    const auto duration = std::chrono::microseconds(_thread_loop_sleep_microseconds);
    while (!_stop_requested) {
        _condition.wait_for(lock, duration);
        if (!_callbacks.empty()) {
            for (auto& pair: _callbacks) {
                pair.second();
            }
        }
    }
}

} // concurrency
} // utils

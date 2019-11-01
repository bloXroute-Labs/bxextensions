#include "utils/concurrency/memory_allocation_thread.h"
#include <chrono>

namespace utils {
namespace concurrency {

MemoryAllocationThread::MemoryAllocationThread():
    _stop_requested(false),
    _last_idx(0)
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

void MemoryAllocationThread::notify(uint32_t idx) {
    { // lock scope
        std::lock_guard<std::mutex> lock(_mtx);
        _events.push(idx);
    }
    _condition.notify_one();
}

void MemoryAllocationThread::_thread_loop() {
    std::unique_lock<std::mutex> lock(_mtx);
    while (!_stop_requested) {
        _condition.wait(lock);
        while (!_events.empty()) {
            uint32_t idx = _events.front();
            auto callback_iter = _callbacks.find(idx);
            if (callback_iter != _callbacks.end()) {
                callback_iter->second();
            }
            _events.pop();
        }
    }
}

} // concurrency
} // utils

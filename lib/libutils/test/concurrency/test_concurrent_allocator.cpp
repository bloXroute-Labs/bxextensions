#include <gtest/gtest.h>
#include <unordered_map>

#include <utils/concurrency/concurrent_allocator.h>
#include <utils/concurrency/memory_allocation_thread.h>


typedef utils::concurrency::ConcurrentAllocator<std::pair<const int, int>> Allocator_t;
typedef utils::concurrency::MemoryAllocationThread MemoryAllocationThread_t;


typedef std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, Allocator_t> Container_t;


class ConcurrentAllocatorTest : public ::testing::Test {
public:

    static const size_t max_allocation_pointer_count;
    static const size_t max_allocations_by_size;

    Container_t& container() {
        return *_container;
    }

    Allocator_t& allocator() {
        return *_allocator;
    }

protected:
    void SetUp() override {
        _allocator = std::unique_ptr<Allocator_t>(new Allocator_t(max_allocation_pointer_count, max_allocations_by_size, &_memory_thread));
        _container = std::unique_ptr<Container_t>(new Container_t(*_allocator));
    }

    MemoryAllocationThread_t _memory_thread;
    std::unique_ptr<Allocator_t> _allocator;
    std::unique_ptr<Container_t> _container;
};

const size_t ConcurrentAllocatorTest::max_allocation_pointer_count = 5;
const size_t ConcurrentAllocatorTest::max_allocations_by_size = 2;



TEST_F(ConcurrentAllocatorTest, test_concurrent_allocation) {
    Container_t& map_container = container();
    Allocator_t& map_allocator = allocator();
    EXPECT_NE(nullptr, map_allocator.executor());
    EXPECT_EQ(map_allocator.executor(), map_container.get_allocator().executor());
    map_container.emplace(4, 8);
    usleep(1000);
    map_container.emplace(77, 98);
    map_container.emplace(54, 2);
    map_container.clear();
}
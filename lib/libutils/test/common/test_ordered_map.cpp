#include <gtest/gtest.h>

#include <vector>
#include <utils/common/ordered_map.h>

typedef utils::common::OrderedMap<int, std::string> OrderedMap_t;

class MovableItem {
public:
    MovableItem(): _ptr(nullptr) {
    }

    MovableItem(const MovableItem& other) = delete;

    MovableItem(MovableItem&& rhs) noexcept {
        _ptr = rhs._ptr;
        rhs._ptr = nullptr;
    }

    MovableItem& operator =(const MovableItem& other) = delete;

    MovableItem& operator =(MovableItem&& rhs) noexcept {
        _ptr = rhs._ptr;
        rhs._ptr = nullptr;
        return *this;
    }

    ~MovableItem() {
        delete _ptr;
    }

    void set_ptr(int val) {
        _ptr = new int;
        *_ptr = val;
    }

    const int* get_ptr() const {
        return _ptr;
    }

private:
    int* _ptr;
};

typedef utils::common::OrderedMap<int, MovableItem> MovableOrderedMap_t;

class OrderedMapTest : public ::testing::Test {
public:

    const std::vector<std::pair<int, std::string>>& items() {
        return _items;
    }

    OrderedMap_t& ordered_map() {
        return _map;
    }

protected:
    void SetUp() {
        _items = {
                {5, "five"},
                {8, "eight"},
                {1, "one"},
                {2, "two"}
        };
        for (const auto& item: _items) {
            _map.emplace(item.first, item.second);
        }
    }

private:
    OrderedMap_t _map;
    std::vector<std::pair<int, std::string>> _items;
};


TEST_F(OrderedMapTest, test_indexing) {
    OrderedMap_t& map = this->ordered_map();
    for (const auto& item: items()) {
        auto iter = map.find(item.first);
        ASSERT_NE(iter, map.end());
        EXPECT_EQ(iter->second, item.second);
        EXPECT_EQ(map[item.first], item.second);
    }
}

TEST_F(OrderedMapTest, test_ordering) {
    const OrderedMap_t& map = this->ordered_map();
    auto iter = map.begin();
    for (const auto& item: items()) {
        ASSERT_NE(iter, map.end());
        EXPECT_EQ(iter->second, item.second);
        ++iter;
    }
}

TEST_F(OrderedMapTest, test_move_semantics) {
    MovableOrderedMap_t map;
    int key = 1, val = 500;
    MovableItem item;
    item.set_ptr(val);
    auto pair = map.emplace(key, std::move(item));
    EXPECT_TRUE(pair.second);
    EXPECT_EQ(key, pair.first->first);
    EXPECT_EQ(val, *pair.first->second.get_ptr());
    EXPECT_EQ(val, *map[key].get_ptr());
}

TEST_F(OrderedMapTest, test_erase) {
    OrderedMap_t& map = this->ordered_map();
    const size_t map_size = map.size();
    for (size_t idx = 0 ; idx < map_size ; ++idx) {
        const auto kvp = items()[idx];
        map.erase(kvp.first);
        EXPECT_EQ(map_size - idx - 1, map.size());
        EXPECT_EQ(map.find(kvp.first), map.end());
    }
}

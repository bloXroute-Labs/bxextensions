#include <gtest/gtest.h>

#include <utils/concurrency/safe_bucket_container.h>

typedef utils::concurrency::SafeBucketContainer<uint32_t> SafeBucketContainer_t;

class SafeBucketContainerTest : public ::testing::Test {
public:

	static const uint32_t bucket_count;
	static const size_t bucket_capacity;

	SafeBucketContainerTest():
		_container(bucket_capacity, bucket_count)
	{
	}

	const SafeBucketContainer_t& read_container(void) const {
		return _container;
	}

	SafeBucketContainer_t& write_container(void) {
		return _container;
	}

	std::unordered_set<uint32_t> ref_set(void) const {
		std::unordered_set<uint32_t> set(
        _ref_set.begin(), _ref_set.end()
		);
		return set;
	}

protected:
	void SetUp() override {
		for (uint32_t i = 0 ; i < bucket_capacity ; ++i) {
			_container.insert(i);
			_ref_set.insert(i);
		}
	}

	void TearDown() override {
		while (_container.is_reading()) {
			_container.release_read();
		}
		_container.stop_requested();
	}

private:
	SafeBucketContainer_t _container;
	std::unordered_set<uint32_t> _ref_set;
};

const uint32_t SafeBucketContainerTest::bucket_count = 3;
const size_t SafeBucketContainerTest::bucket_capacity = 10;


TEST_F(SafeBucketContainerTest, test_fill_single_bucket) {
	SafeBucketContainer_t& w_container = write_container();
	w_container.acquire_read();
	const SafeBucketContainer_t& r_container = read_container();
	ASSERT_EQ(r_container.size(), bucket_capacity);
	EXPECT_FALSE(r_container[0].empty());
	EXPECT_TRUE(r_container[0].is_full());
	std::unordered_set<uint32_t> ref_items = ref_set();
	for(const uint32_t& item: r_container[0]) {
		EXPECT_TRUE(ref_items.find(item) != ref_items.end());
		ref_items.erase(item);
	}
	for(uint32_t i = 1 ; i < bucket_count ; ++i) {
		EXPECT_EQ(r_container[i].size(), 0);
	}
}

TEST_F(SafeBucketContainerTest, test_fill_multiple_buckets) {
	SafeBucketContainer_t& w_container = write_container();
	std::unordered_set<uint32_t> ref_items;
	for (
			uint32_t i = bucket_capacity ;
			i - bucket_capacity < bucket_capacity / 2 ;
			++i
	)
	{
		w_container.insert(i);
		ref_items.insert(i);
	}
	w_container.acquire_read();
	const SafeBucketContainer_t& r_container = read_container();
	EXPECT_FALSE(r_container[0].empty());
	EXPECT_TRUE(r_container[0].is_full());
	for(const uint32_t& item: r_container[1]) {
		EXPECT_TRUE(ref_items.find(item) != ref_items.end());
		ref_items.erase(item);
	}

	for(uint32_t i = 2 ; i < bucket_count ; ++i) {
		EXPECT_TRUE(r_container[i].empty());
	}
}

TEST_F(SafeBucketContainerTest, test_cap_container) {
	SafeBucketContainer_t& w_container = write_container();
	size_t item_count = (bucket_count - 1) * bucket_capacity + 2;
	for (size_t i = 0 ; i < item_count ; ++i) {
		w_container.insert(i + bucket_capacity);
	}
	w_container.acquire_read();
	const SafeBucketContainer_t& r_container = read_container();
	ASSERT_EQ(r_container.size(), bucket_capacity * bucket_count + 2);
	EXPECT_EQ(r_container[0].size(), bucket_capacity + 1);
	EXPECT_EQ(r_container[1].size(), bucket_capacity + 1);
	for (uint32_t i = 2 ; i < bucket_count ; ++i) {
		EXPECT_EQ(r_container[i].size(), bucket_capacity);
		EXPECT_FALSE(r_container[i].empty());
		EXPECT_TRUE(r_container[i].is_full());
	}
}

TEST_F(SafeBucketContainerTest, test_erase_no_lock) {
	SafeBucketContainer_t& w_container = write_container();
	const uint32_t item_to_delete = bucket_capacity - 1;
	const SafeBucketContainer_t& r_container = read_container();
	EXPECT_TRUE(r_container[0].contains(item_to_delete));
	w_container.erase(item_to_delete);
	EXPECT_FALSE(r_container[0].contains(item_to_delete));
}

TEST_F(SafeBucketContainerTest, test_single_read_lock) {
	SafeBucketContainer_t& w_container = write_container();
	const uint32_t item = 10001;
	const uint32_t item_to_delete = bucket_capacity - 1;
	w_container.acquire_read();
	const SafeBucketContainer_t& r_container = read_container();
	EXPECT_TRUE(r_container.is_reading());
	EXPECT_FALSE(r_container.is_write_pending());
	w_container.insert(item);
	w_container.erase(item_to_delete);
	EXPECT_TRUE(r_container.is_write_pending());
	EXPECT_FALSE(r_container[1].contains(item));
	EXPECT_TRUE(r_container[0].contains(item_to_delete));
	w_container.release_read();
	EXPECT_FALSE(r_container.is_reading());
	EXPECT_FALSE(r_container.is_write_pending());
	EXPECT_TRUE(r_container[1].contains(item));
	EXPECT_FALSE(r_container[0].contains(item_to_delete));
}

TEST_F(SafeBucketContainerTest, test_clear) {
	SafeBucketContainer_t& w_container = write_container();
    w_container.clear();
}
#include <gtest/gtest.h>

#include <vector>
#include <utils/common/default_map.h>
#include <utils/exception/key_error.h>

typedef utils::common::DefaultMap<int, std::string> DefaultMap_t;
typedef utils::common::DefaultMap<int, std::vector<std::string>> DefaultNestedMap_t;
typedef utils::exception::KeyError KeyError_t;

#define FIRST_INDEX 5
#define SECOND_INDEX 23
#define FIRST_VALUE "hello one"
#define SECOND_VALUE "hello two"

class DefaultMapTest : public ::testing::Test {
public:
	static const std::string empty;

	DefaultMap_t& map() {
		return _map;
	}
	const DefaultMap_t& const_map() const {
		return _const_map;
	}
	DefaultNestedMap_t& nested_map() {
		return _nested_map;
	}

protected:
	void SetUp() {
		_const_map[FIRST_INDEX] = FIRST_VALUE;
		_const_map[SECOND_INDEX] = SECOND_VALUE;
	}

private:
	DefaultMap_t _map;
	DefaultMap_t _const_map;
	DefaultNestedMap_t _nested_map;
};

const std::string DefaultMapTest::empty = "";

struct nested_vector_factory {
	std::vector<std::string> operator()() {
		std::vector<std::string> vec = {FIRST_VALUE, SECOND_VALUE};
		return vec;
	}
};

typedef utils::common::DefaultMap<int, std::vector<std::string>, std::hash<int>, std::equal_to<int>, nested_vector_factory> DefaultFactoryMap_t;


TEST_F(DefaultMapTest, test_indexing) {
	const DefaultMap_t& const_map = this->const_map();
	DefaultMap_t& map = this->map();
	EXPECT_EQ(const_map[FIRST_INDEX], FIRST_VALUE);
	EXPECT_EQ(const_map[SECOND_INDEX], SECOND_VALUE);
	EXPECT_THROW(const_map[18], KeyError_t);
	EXPECT_EQ(map[6], empty);
}

TEST_F(DefaultMapTest, test_nested) {
	DefaultNestedMap_t& nested_map = this->nested_map();
	ASSERT_NO_THROW(nested_map[FIRST_INDEX].push_back(FIRST_VALUE));
	EXPECT_EQ(nested_map[FIRST_INDEX][0], FIRST_VALUE);
}

TEST_F(DefaultMapTest, test_value_factory) {
	nested_vector_factory factory;
	DefaultFactoryMap_t map(factory);
	EXPECT_EQ(map[5], factory());
}

TEST_F(DefaultMapTest, test_find) {
	const DefaultMap_t& const_map = this->const_map();
	DefaultMap_t& map = this->map();
	DefaultNestedMap_t nested_map;
	EXPECT_EQ(const_map.find(FIRST_INDEX)->second, FIRST_VALUE);
	EXPECT_EQ(const_map.find(SECOND_INDEX)->second, SECOND_VALUE);
	EXPECT_EQ(const_map.find(13), const_map.end());
	EXPECT_EQ(map.find(13), map.end());
	EXPECT_EQ(nested_map.find(13), nested_map.end());
	nested_map[13].push_back("hey");
	EXPECT_EQ(*nested_map.find(13)->second.begin(), "hey");
}

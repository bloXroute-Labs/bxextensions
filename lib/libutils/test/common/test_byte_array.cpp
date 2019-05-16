#include <gtest/gtest.h>
#include <array>

#include <utils/common/byte_array.h>
#include <utils/exception/index_error.h>

typedef utils::exception::IndexError IndexError_t;


#define CAPACITY 16
#define FROM 5
#define SIZE 9

typedef utils::common::ByteArray ByteArray_t;

class ByteArrayTest : public ::testing::Test {
public:
	static const std::array<uint8_t, CAPACITY> array;

	const ByteArray_t& byte_array() const {
		return _byte_array;
	}

	ByteArray_t& byte_array() {
		return _byte_array;
	}

protected:
	void SetUp() override {
		_byte_array = ByteArray_t(CAPACITY);
		_byte_array.from_char_array(
				(const char*)&array[FROM], SIZE, FROM
		);
	}

private:
	ByteArray_t _byte_array;
};

const std::array<uint8_t, CAPACITY> ByteArrayTest::array = {
		0, 0, 0, 0, 22, 15, 255, 234, 11, 88, 0, 0, 15, 0, 3, 0
};


TEST_F(ByteArrayTest, test_init) {
	const ByteArray_t& ref_array = byte_array();

	// check the size of the array
	ASSERT_EQ(ref_array.size(), SIZE + FROM);

	// check the array content
	for (size_t idx = FROM ; idx < SIZE ; ++idx) {
		EXPECT_EQ(ref_array.at(idx), array[idx]);
	}

	// check the array padding
	for (size_t idx = 0 ; idx < FROM ; ++idx) {
		EXPECT_EQ(ref_array.at(idx), 0);
	}
}


TEST_F(ByteArrayTest, test_indexing) {
	const ByteArray_t& ref_array = byte_array();

	// check that index error is being thrown when exceeding the array size
	EXPECT_THROW(ref_array.at(SIZE + FROM), IndexError_t);

	/*
	 * check that no errors are thrown when accessing the
	 * first and last values in the array
	 */
	EXPECT_NO_THROW(ref_array.at(SIZE));
	EXPECT_NO_THROW(ref_array.at(FROM - 1));
}


TEST_F(ByteArrayTest, test_set_val) {
	const size_t idx = FROM + 5;
	const uint8_t val = 254;
	auto& buffer = byte_array();
	const uint8_t original_val = buffer.at(idx);
	buffer[idx] = val;
	EXPECT_EQ(buffer.at(idx), val);
	EXPECT_EQ(array.at(idx), original_val);
	EXPECT_NE(array.at(idx), val);
}

TEST_F(ByteArrayTest, test_append) {
	ByteArray_t& ref_array = byte_array();
	ByteArray_t sub_array(20);
	const std::string suffix("buffer suffix testing");
	sub_array.from_str(suffix);

	// check for value and non reference equalities on sub array
	ASSERT_NE(suffix.c_str(), sub_array.char_array());
	ASSERT_EQ(suffix, std::string(sub_array.char_array()));
	const size_t original_size = ref_array.size();
	ref_array += sub_array;

	// check array size after the append
	EXPECT_EQ(ref_array.size(), original_size + sub_array.size());

	// check for value and non reference equalities on ref array
	EXPECT_NE(
			&ref_array.char_array()[original_size],
			sub_array.char_array()
	);
	EXPECT_EQ(std::string(&ref_array.char_array()[original_size]), suffix);
}

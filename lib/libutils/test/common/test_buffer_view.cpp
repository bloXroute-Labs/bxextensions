#include <gtest/gtest.h>
#include <array>

#include <utils/common/buffer_view.h>
#include <utils/exception/index_error.h>

typedef utils::exception::IndexError IndexError_t;

#define BUFFER_LEN 16
#define FROM 4
#define SIZE 10

typedef utils::common::BufferView BufferView_t;

class BufferViewTest : public ::testing::Test {
public:
	static const std::array<uint8_t, BUFFER_LEN> array;

	const BufferView_t& buffer_view() const {
		return _buffer_view;
	}

protected:
	void SetUp() override {
		_buffer_view = BufferView_t(
				&array.at(0), SIZE, FROM
		);
	}

private:
	BufferView_t _buffer_view;
};

const std::array<uint8_t, BUFFER_LEN> BufferViewTest::array = {
		0, 0, 0, 0, 22, 15, 255, 234, 11, 88, 0, 0, 15, 0, 3, 0
};


TEST_F(BufferViewTest, test_init) {
	const BufferView_t& buffer = buffer_view();
	// checking buffer size
	ASSERT_EQ(buffer.size(), SIZE);

	// checking buffer underlying pointer
	ASSERT_EQ(buffer.char_array(), (const char*)&array[FROM]);
	ASSERT_EQ(buffer.byte_array(), &array[FROM]);

	// checking bool operator
	EXPECT_FALSE(BufferView_t::empty());
	EXPECT_TRUE(buffer);

	// checking buffer slice content
	for (size_t idx = FROM ; idx < SIZE ; ++idx) {
		EXPECT_EQ(buffer[idx - FROM], array[idx]);
	}
}


/*
 * checking buffer view iterator
 */
TEST_F(BufferViewTest, test_iter) {
	const BufferView_t& buffer = buffer_view();
	size_t idx = FROM;
	for (auto it = buffer.begin() ; it != buffer.end() ; ++it) {
		ASSERT_EQ(*it, array[idx]);
		ASSERT_LT(idx, BUFFER_LEN);
		++idx;
	}
}


TEST_F(BufferViewTest, test_indexing) {
	const BufferView_t& buffer = buffer_view();

	// checking the over the last index value of the buffer
	EXPECT_EQ(buffer[SIZE], array[SIZE + FROM]);

	// check that index error is being thrown when exceeding the buffer size
	EXPECT_THROW(buffer.at(SIZE), IndexError_t);

	/*
	 * check that no errors thrown when accessing the the
	 * first and last values in the buffer
	 */
	EXPECT_NO_THROW(buffer.at(SIZE - 1));
	EXPECT_NO_THROW(buffer.at(0));
}

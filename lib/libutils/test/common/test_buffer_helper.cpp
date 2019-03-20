#include <gtest/gtest.h>

#include <utils/common/buffer_helper.h>

template <typename T>
static std::vector<uint8_t>& set_le_val(
		T val, std::vector<uint8_t>& buf, size_t offset
)
{
	constexpr size_t type_size = sizeof(T);
	size_t end = offset + type_size;
	if (buf.size() < end) {
		buf.resize(end);
	}
	memcpy(
			&buf.at(offset),
			(uint8_t*)&val,
			type_size
	);
	return buf;
}

class BufferHelperTest : public ::testing::Test {
public:
	const std::vector<uint8_t>& get_buf() const {
		return _buf;
	}

	const uint8_t max_byte_val = 255, min_byte_val = 0;
	const short max_short_val = 32767, min_short_val = -32768;
	const int max_int_val = 2147483647, min_int_val = -2147483648;
	const int64_t max_long_int_val = 9223372036854775807,
			min_long_int_val= -9223372036854775808;

protected:
	void SetUp() override {
		_buf.reserve(500);
		set_le_val(max_byte_val, _buf, _buf.size());
		set_le_val(min_byte_val, _buf, _buf.size());

		set_le_val(max_short_val, _buf, _buf.size());
		set_le_val(min_short_val, _buf, _buf.size());

		set_le_val(max_int_val, _buf, _buf.size());
		set_le_val(min_int_val, _buf, _buf.size());

		set_le_val(max_long_int_val, _buf, _buf.size());
		set_le_val(min_long_int_val, _buf, _buf.size());
	}

private:
	std::vector<uint8_t> _buf;
};

TEST_F(BufferHelperTest, test_get_little_endian_value) {

	size_t offset = 0;
	uint8_t val = 11;
	const std::vector<uint8_t>& buf = get_buf();
	offset = utils::common::get_little_endian_value<uint8_t>(
			buf, val, offset
	);
	ASSERT_EQ(max_byte_val, val);
	offset = utils::common::get_little_endian_value<uint8_t>(
			buf, val, offset
	);
	ASSERT_EQ(min_byte_val, val);

	short short_val = 11;
	offset = utils::common::get_little_endian_value<short>(
			buf, short_val, offset
	);
	ASSERT_EQ(max_short_val, short_val);
	offset = utils::common::get_little_endian_value<short>(
			buf, short_val, offset
	);
	ASSERT_EQ(min_short_val, short_val);

	int int_val = 11;
	offset = utils::common::get_little_endian_value<int>(
			buf, int_val, offset
	);
	ASSERT_EQ(max_int_val, int_val);
	offset = utils::common::get_little_endian_value<int>(
			buf, int_val, offset
	);
	ASSERT_EQ(min_int_val, int_val);

	int64_t long_int_val = 11;
	offset = utils::common::get_little_endian_value<int64_t>(
			buf, long_int_val, offset
	);
	ASSERT_EQ(max_long_int_val, long_int_val);
	offset = utils::common::get_little_endian_value<int64_t>(
			buf, long_int_val, offset
	);
	ASSERT_EQ(min_long_int_val, long_int_val);
}


TEST_F(BufferHelperTest, test_set_little_endian_value) {
	std::vector<uint8_t> buf;
	buf.reserve(500);
	utils::common::set_little_endian_value(
			buf, max_byte_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, min_byte_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, max_short_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, min_short_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, max_int_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, min_int_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, max_long_int_val, buf.size()
	);
	utils::common::set_little_endian_value(
			buf, min_long_int_val, buf.size()
	);

	ASSERT_EQ(get_buf(),buf);
}

#include <gtest/gtest.h>
#include <array>

#include <utils/common/buffer_helper.h>
#include <utils/common/string_helper.h>

template <typename T>
static size_t set_le_val(
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
	return end;
}

template <typename T>
static size_t set_be_val(
		T val, std::vector<uint8_t>& buf, size_t offset
)
{
	constexpr size_t type_size = sizeof(T);
	size_t end = offset + type_size;
	if (buf.size() < end) {
		buf.resize(end);
	}
	std::array<uint8_t, type_size> val_array;
	memcpy(
			&val_array.at(0),
			(uint8_t*)&val,
			type_size
	);
	std::reverse(val_array.begin(), val_array.end());
	memcpy(
			&buf.at(offset),
			&val_array.at(0),
			type_size
	);
	return end;
}


class BufferHelperTest : public ::testing::Test {
public:
	static const size_t base_offset;

	const std::vector<uint8_t>& get_buf() const {
		return _buf;
	}

	const std::vector<uint8_t>& get_be_buf() const {
		return _be_buf;
	}

	const uint8_t max_byte_val = std::numeric_limits<uint8_t>::max();
	const uint8_t min_byte_val = std::numeric_limits<uint8_t>::min();
	const short max_short_val = std::numeric_limits<short>::max();
	const short min_short_val = std::numeric_limits<short>::min();
	const int max_int_val = std::numeric_limits<int>::max();
	const int min_int_val = std::numeric_limits<int>::min();
	const int64_t max_long_int_val = std::numeric_limits<int64_t>::max();
	const int64_t min_long_int_val = std::numeric_limits<int64_t>::min();

protected:
	void SetUp() override {
		_buf.reserve(500);
		size_t offset = set_le_val(max_byte_val, _buf, base_offset);
		offset = set_le_val(min_byte_val, _buf, offset);
		offset = set_le_val(max_short_val, _buf, offset);
		offset = set_le_val(min_short_val, _buf, offset);
		offset = set_le_val(max_int_val, _buf, offset);
		offset = set_le_val(min_int_val, _buf, offset);
		offset = set_le_val(max_long_int_val, _buf, offset);
		offset = set_le_val(min_long_int_val, _buf, offset);

		_be_buf.reserve(500);
		offset = offset = set_be_val(max_byte_val, _be_buf, base_offset);
		offset = set_be_val(min_byte_val, _be_buf, offset);
		offset = set_be_val(max_short_val, _be_buf, offset);
		offset = set_be_val(min_short_val, _be_buf, offset);
		offset = set_be_val(max_int_val, _be_buf, offset);
		offset = set_be_val(min_int_val, _be_buf, offset);
		offset = set_be_val(max_long_int_val, _be_buf, offset);
		offset = set_be_val(min_long_int_val, _be_buf, offset);
	}

private:
	std::vector<uint8_t> _buf, _be_buf;
};

const size_t BufferHelperTest::base_offset = 5;


TEST_F(BufferHelperTest, test_get_little_endian_value) {

	size_t offset = 0;

	// checking byte parsing (LE)
	uint8_t val = 11;
	const std::vector<uint8_t>& buf = get_buf();
	offset = utils::common::get_little_endian_value<uint8_t>(
			buf, val, base_offset
	);

	ASSERT_EQ(max_byte_val, val);
	offset = utils::common::get_little_endian_value<uint8_t>(
			buf, val, offset
	);
	ASSERT_EQ(min_byte_val, val);

	// checking short parsing (LE)
	short short_val = 11;
	offset = utils::common::get_little_endian_value<short>(
			buf, short_val, offset
	);
	ASSERT_EQ(max_short_val, short_val);
	offset = utils::common::get_little_endian_value<short>(
			buf, short_val, offset
	);
	ASSERT_EQ(min_short_val, short_val);

	// checking integer parsing (LE)
	int int_val = 11;
	offset = utils::common::get_little_endian_value<int>(
			buf, int_val, offset
	);
	ASSERT_EQ(max_int_val, int_val);
	offset = utils::common::get_little_endian_value<int>(
			buf, int_val, offset
	);
	ASSERT_EQ(min_int_val, int_val);

	// checking 8 byte integer parsing (LE)
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


TEST_F(BufferHelperTest, test_get_big_endian_value) {

	size_t offset = 0;

	// checking byte parsing (BE)
	uint8_t val = 11;
	const std::vector<uint8_t>& buf = get_be_buf();
	offset = utils::common::get_big_endian_value<uint8_t>(
			buf, val, base_offset
	);
	ASSERT_EQ(max_byte_val, val);
	offset = utils::common::get_big_endian_value<uint8_t>(
			buf, val, offset
	);
	ASSERT_EQ(min_byte_val, val);

	// checking short parsing (BE)
	short short_val = 11;
	offset = utils::common::get_big_endian_value<short>(
			buf, short_val, offset
	);
	ASSERT_EQ(max_short_val, short_val);
	offset = utils::common::get_big_endian_value<short>(
			buf, short_val, offset
	);
	ASSERT_EQ(min_short_val, short_val);

	// checking integer parsing (BE)
	int int_val = 11;
	offset = utils::common::get_big_endian_value<int>(
			buf, int_val, offset
	);
	ASSERT_EQ(max_int_val, int_val);
	offset = utils::common::get_big_endian_value<int>(
			buf, int_val, offset
	);
	ASSERT_EQ(min_int_val, int_val);


	// checking 8 byte integer parsing (BE)
	int64_t long_int_val = 11;
	offset = utils::common::get_big_endian_value<int64_t>(
			buf, long_int_val, offset
	);
	ASSERT_EQ(max_long_int_val, long_int_val);
	offset = utils::common::get_big_endian_value<int64_t>(
			buf, long_int_val, offset
	);
	ASSERT_EQ(min_long_int_val, long_int_val);
}


TEST_F(BufferHelperTest, test_set_little_endian_value) {
	std::vector<uint8_t> buf;
	buf.reserve(500);

	// checking byte packing (LE)
	size_t offset = utils::common::set_little_endian_value(
			buf, max_byte_val, base_offset
	);
	offset = utils::common::set_little_endian_value(
			buf, min_byte_val, offset
	);

	// checking short packing (LE)
	offset = utils::common::set_little_endian_value(
			buf, max_short_val, offset
	);
	offset = utils::common::set_little_endian_value(
			buf, min_short_val, offset
	);

	// checking integer packing (LE)
	offset = utils::common::set_little_endian_value(
			buf, max_int_val, offset
	);
	offset = utils::common::set_little_endian_value(
			buf, min_int_val, offset
	);

	// checking 8 byte integer packing (LE)
	offset = utils::common::set_little_endian_value(
			buf, max_long_int_val, offset
	);
	utils::common::set_little_endian_value(
			buf, min_long_int_val, offset
	);

	ASSERT_EQ(get_buf(), buf);
}

TEST_F(BufferHelperTest, test_get_compact_short_id) {
    uint64_t desired = 0x1c31097bfc15;
    uint64_t src = 0;
    std::string source = "15fc7b09311c";
    std::vector <uint8_t> source_vec;
    utils::common::from_hex_string(source, source_vec);
    utils::common::BufferView source_buffer_view(source_vec);
    get_compact_short_id(source_buffer_view, src, 0);
    ASSERT_EQ(desired, src);
}
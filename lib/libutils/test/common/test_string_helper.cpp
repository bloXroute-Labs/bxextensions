#include <gtest/gtest.h>

#include <utils/common/string_helper.h>

/*
 * checking bytes to hex string conversion
 */
TEST(StringHelperTest, test_to_hex_string) {
	const std::string hex_string("05ffa3020100");
	const std::vector<uint8_t> buf = {
			0x05, 0xff, 0xa3, 0x02, 0x01, 0x00
	};
	ASSERT_EQ(hex_string, utils::common::to_hex_string(buf));
}

/*
 * checking hex string to bytes conversion
 */
TEST(StringHelperTest, test_from_hex_string) {
	const std::string hex_string("05ffa3020100");
	const std::vector<uint8_t> ref_buf = {
			0x05, 0xff, 0xa3, 0x02, 0x01, 0x00
	};
	std::vector<uint8_t> buf;
	utils::common::from_hex_string(hex_string, buf);
	ASSERT_EQ(ref_buf, buf);
}

/*
 * test concatenate strings
 */
TEST(StringHelperTest, test_concatenate) {
	const std::string str1("foo "), str2("bar "), str3("...");
	std::string concat = utils::common::concatenate(str1, str2, str3);
	ASSERT_EQ(str1 + str2 + str3, concat);
}




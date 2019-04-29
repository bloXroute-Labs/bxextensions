#include <gtest/gtest.h>

#include <utils/common/string_helper.h>

/*
 * checking byte to hex string conversion
 */
TEST(StringHelperTest, test_to_hex_string) {
	const std::string hex_string("05ffa3020100");
	const std::vector<uint8_t> buf = {
			0x05, 0xff, 0xa3, 0x02, 0x01, 0x00
	};
	ASSERT_EQ(hex_string, utils::common::to_hex_string(buf));
}

/*
 * test concatinate strings
 */
TEST(StringHelperTest, test_concatinate) {
	const std::string str1("foo "), str2("bar "), str3("...");
	std::string concat = utils::common::concatinate(str1, str2, str3);
	ASSERT_EQ(str1 + str2 + str3, concat);
}




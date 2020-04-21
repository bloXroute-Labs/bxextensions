#include <gtest/gtest.h>
#include <iostream>

#include "utils/encoding/base64_encoder.h"
#include "utils/common/byte_array.h"

typedef utils::common::ByteArray ByteArray_t;

class Base64EncoderTest : public ::testing::Test {
public:
    static const std::string raw_str;
    static const std::string encoded_str;
};

const std::string Base64EncoderTest::raw_str = "test base64 encoding";

const std::string Base64EncoderTest::encoded_str = "dGVzdCBiYXNlNjQgZW5jb2Rpbmc=";


TEST_F(Base64EncoderTest, test_basic_encoding) {
    std::string b64_encoded_str = utils::encoding::base64_encode(raw_str);
    EXPECT_EQ(encoded_str, b64_encoded_str);
    ByteArray_t buf(raw_str.size());
    utils::encoding::base64_decode(encoded_str, buf, 0);
    std::string decoded_str(buf.char_array());
    EXPECT_EQ(raw_str, decoded_str);
}
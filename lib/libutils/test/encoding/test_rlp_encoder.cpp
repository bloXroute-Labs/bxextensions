#include <vector>
#include <gtest/gtest.h>
#include <utils/common/byte_array.h>
#include <utils/encoding/rlp_encoder.h>
#include <utils/exception/parser_error.h>

typedef utils::common::ByteArray ByteArray_t;

class RlpEncoderTest : public ::testing::Test {
public:
    ByteArray_t& array(){ return _array; }
private:
    ByteArray_t _array;
};


TEST_F(RlpEncoderTest, test_encode_int_byte) {
    uint64_t value = 50;
    uint8_t output_value = 0;
    size_t offset = utils::encoding::rlp_encode(array(), value, 0);
    size_t output_offset = utils::common::get_big_endian_value<uint8_t>(array(), output_value, 0);
    EXPECT_EQ(value, output_value);
    EXPECT_EQ(offset, output_offset);
}

TEST_F(RlpEncoderTest, test_encode_int_long) {
    uint64_t value = 1000000;
    uint64_t output_value = 0;
    size_t starting_offset = 1;
    size_t offset = utils::encoding::rlp_encode(array(), value, 0);
    size_t output_offset = utils::encoding::get_big_endian_rlp_value(
        array(), output_value, starting_offset, offset - starting_offset
    );
    EXPECT_EQ(value, output_value);
    EXPECT_EQ(offset, output_offset);
}


TEST_F(RlpEncoderTest, test_encode_int_long_long) {
    uint64_t value = 100000000000;
    uint64_t output_value = 0;
    size_t starting_offset = 1;
    size_t offset = utils::encoding::rlp_encode(array(), value, 0);
    size_t output_offset = utils::encoding::get_big_endian_rlp_value(
            array(), output_value, starting_offset, offset - starting_offset
    );
    EXPECT_EQ(value, output_value);
    EXPECT_EQ(offset, output_offset);
}

TEST_F(RlpEncoderTest, test_consume_length_prefix_0) {
    uint64_t length = 0;
    const std::vector<uint8_t> vec = {0x81, 0x00};
    array().from_array(vec);
    EXPECT_THROW(
            utils::encoding::consume_length_prefix(array(), length, 0),
            utils::exception::ParserError
    );

}

TEST_F(RlpEncoderTest, test_consume_length_prefix_1) {
    uint64_t length = 0;
    const std::vector<uint8_t> vec = {0xc2, 0x80, 0x80};
    array().from_array(vec);
    size_t offset = utils::encoding::consume_length_prefix(array(), length, 0);
    ASSERT_EQ(length, 2);
    ASSERT_EQ(offset, 1);
}

TEST_F(RlpEncoderTest, test_consume_length_prefix_2) {
    uint64_t length = 0;
    const std::vector<uint8_t> vec = {0xb8, 0x38};
    array().from_array(vec);
    size_t offset = utils::encoding::consume_length_prefix(array(), length, 0);
    ASSERT_EQ(length, 56);
    ASSERT_EQ(offset, 2);
}

TEST_F(RlpEncoderTest, test_consume_length_prefix_3) {
    uint64_t length = 0;
    const std::vector<uint8_t> vec = {0xb8, 0x80};
    array().from_array(vec);
    size_t offset = utils::encoding::consume_length_prefix(array(), length, 0);
    ASSERT_EQ(length, 128);
    ASSERT_EQ(offset, 2);
}

TEST_F(RlpEncoderTest, test_consume_length_prefix_4) {
    uint64_t length = 0;
    const std::vector<uint8_t> vec = {0xb9, 0x04, 0x00};
    array().from_array(vec);
    size_t offset = utils::encoding::consume_length_prefix(array(), length, 0);
    ASSERT_EQ(length, 1024);
    ASSERT_EQ(offset, 3);
}

TEST_F(RlpEncoderTest, test_consume_length_prefix_5) {
    uint64_t length = 0;
    const std::vector<uint8_t> vec = {0xb9, 0x04, 0x00};
    array().from_array(vec);
    size_t offset = utils::encoding::consume_length_prefix(array(), length, 0);
    ASSERT_EQ(length, 1024);
    ASSERT_EQ(offset, 3);
}

TEST_F(RlpEncoderTest, test_rlp_decode_127) {
    uint64_t value = 127;
    uint64_t output_value = 0;
    utils::encoding::rlp_encode(array(), value, 0);
    size_t offset = utils::encoding::rlp_decode(array(), output_value, 0);
    ASSERT_EQ(output_value, 127);
    ASSERT_EQ(offset, 1);
}

TEST_F(RlpEncoderTest, test_rlp_decode_0) {
    uint64_t value = 0;
    uint64_t output_value = 0;
    utils::encoding::rlp_encode(array(), value, 0);
    size_t offset = utils::encoding::rlp_decode(array(), output_value, 0);
    ASSERT_EQ(output_value, 0);
    ASSERT_EQ(offset, 1);
}

TEST_F(RlpEncoderTest, test_rlp_decode_128) {
    uint64_t value = 128;
    uint64_t output_value = 0;
    utils::encoding::rlp_encode(array(), value, 0);
    size_t offset = utils::encoding::rlp_decode(array(), output_value, 0);
    ASSERT_EQ(output_value, 128);
    ASSERT_EQ(offset, 2);
}

TEST_F(RlpEncoderTest, test_rlp_decode_256) {
    uint64_t value = 256;
    uint64_t output_value = 0;
    utils::encoding::rlp_encode(array(), value, 0);
    size_t offset = utils::encoding::rlp_decode(array(), output_value, 0);
    ASSERT_EQ(output_value, 256);
    ASSERT_EQ(offset, 3);
}

TEST_F(RlpEncoderTest, test_rlp_decode_1M) {
    uint64_t value = 1000000;
    uint64_t output_value = 0;
    utils::encoding::rlp_encode(array(), value, 0);
    size_t offset = utils::encoding::rlp_decode(array(), output_value, 0);
    ASSERT_EQ(output_value, 1000000);
    ASSERT_EQ(offset, 4);
}

TEST_F(RlpEncoderTest, test_get_length_prefix_str_1) {
    uint8_t value = 1;
    const std::vector<uint8_t> vec = {0x81};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_str(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_str_2) {
    uint8_t value = 2;
    const std::vector<uint8_t> vec = {0x82};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_str(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_str_56) {
    uint8_t value = 56;
    const std::vector<uint8_t> vec = {0xb8, 0x38};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_str(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_str_128) {
    uint8_t value = 128;
    const std::vector<uint8_t> vec = {0xb8, 0x80};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_str(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_str_1024) {
    uint16_t value = 1024;
    const std::vector<uint8_t> vec = {0xb9, 0x04, 0x00};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_str(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_list_1) {
    uint8_t value = 1;
    const std::vector<uint8_t> vec = {0xc1};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_list(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_list_2) {
    uint8_t value = 2;
    const std::vector<uint8_t> vec = {0xc2};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_list(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_list_56) {
    uint8_t value = 56  ;
    const std::vector<uint8_t> vec = {0xf8, 0x38};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_list(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_list_128) {
    uint8_t value = 128;
    const std::vector<uint8_t> vec = {0xf8, 0x80};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_list(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

TEST_F(RlpEncoderTest, test_get_length_prefix_list_1024) {
    uint16_t value = 1024;
    const std::vector<uint8_t> vec = {0xf9, 0x04, 0x00};
    ByteArray_t expected_array = ByteArray_t();
    expected_array.from_array(vec);
    utils::encoding::get_length_prefix_list(array(), value, 0);
    ASSERT_EQ(*(array().char_array()), *(expected_array.char_array()));
}

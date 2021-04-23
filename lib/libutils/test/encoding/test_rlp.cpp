#include <vector>
#include <gtest/gtest.h>
#include <utils/common/byte_array.h>
#include <utils/encoding/rlp.h>

typedef utils::common::ByteArray ByteArray_t;
typedef utils::common::BufferView BufferView_t;

class RlpTest : public ::testing::Test {
public:
    ByteArray_t& array(){ return _array; }
private:
    ByteArray_t _array;
};


TEST_F(RlpTest, test_decode) {
    std::string string_val("rlp test string");
    std::vector<std::string> list_val = {"test_1", "test_2", "test_3", "test_4"};
    std::string encoded_rlp("\x82\xd9\x03\x80\x8frlp test string\xdc\x86test_1\x86test_2\x86test_3\x86test_4");
    array().resize(encoded_rlp.size());
    memcpy(array().char_array(), encoded_rlp.c_str(), encoded_rlp.size());
    BufferView_t rlp_buf(array().byte_array(), array().size());

    utils::encoding::Rlp rlp(rlp_buf,  20);
    ASSERT_EQ(RLP_LIST, rlp.rlp_type());
    auto rlp_list = rlp.get_rlp_list();
    auto list_iter = list_val.begin();
    ASSERT_EQ(list_val.size(), rlp_list.size());
    for (auto &rlp_item: rlp_list) {
        ASSERT_EQ(*list_iter, std::string(rlp_item.as_rlp_string().char_array(), rlp_item.length()));
        ++list_iter;
    }
}

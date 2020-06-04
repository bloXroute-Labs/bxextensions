#include <vector>
#include <gtest/gtest.h>
#include <utils/common/byte_array.h>
#include <utils/protocols/ethereum/eth_tx_message.h>
#include <utils/common/string_helper.h>

typedef utils::common::ByteArray ByteArray_t;
typedef utils::common::BufferView BufferView_t;
typedef utils::protocols::ethereum::EthTxMessage EthTxMessage_t;

class EthTxMessageTest : public ::testing::Test {
};

TEST_F(EthTxMessageTest, test_tx_decoding) {
    std::string rlp_str = "050a0f94759b974eb29c155aa9d30fe54c00205c2729cd9014b84bd2347cdfff0337bbf56cc31ec8ccb87b90a9b701a4f6aaa296348bbf91f5ad9a3bea8f8a54b54d6e880cadfb015eb04b94ce88afef04ccdc9c69eedaf51318a7d9e94e5f9fa38c566ae342191e23";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    std::string address_str = "759b974eb29c155aa9d30fe54c00205c2729cd90";
    std::string data_str = "d2347cdfff0337bbf56cc31ec8ccb87b90a9b701a4f6aaa296348bbf91f5ad9a3bea8f8a54b54d6e880cadfb015eb04b94ce88afef04ccdc9c69eedaf51318a7d9e94e5f9fa38c566ae342";
    EXPECT_EQ(5, msg.nonce());
    EXPECT_EQ(10, msg.gas_price());
    EXPECT_EQ(15, msg.start_gas());
    EXPECT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    EXPECT_EQ(20, msg.value());
    EXPECT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    EXPECT_EQ(25, msg.v());
    EXPECT_EQ(30, msg.r());
    EXPECT_EQ(35, msg.s());
}
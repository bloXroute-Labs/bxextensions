#include <vector>
#include <gtest/gtest.h>
#include <utils/common/byte_array.h>
#include <utils/protocols/ethereum/eth_tx_message.h>
#include <utils/common/string_helper.h>
#include <utils/crypto/signature.h>

typedef utils::common::ByteArray ByteArray_t;
typedef utils::common::BufferView BufferView_t;
typedef utils::protocols::ethereum::EthTxMessage EthTxMessage_t;

class EthTxMessageTest : public ::testing::Test {
};

TEST_F(EthTxMessageTest, test_tx_decoding) {
    std::string rlp_str = "f86b21850bdfd63e0082520894f8044ff824c2dce174b4ee9f958c2a738483189e87093cafac6a80008026a02dbf2ca92baeab4a03cdfae33cbf240065e24e7cc9f7e2a99c3edf6e0c4fc016a029114b3d3b96587d61d5000665537ad12ce43ea18cf87f3e0e3ad1cd003f2715";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    std::string address_str = "f8044ff824c2dce174b4ee9f958c2a738483189e";
    std::string data_str;
    EXPECT_EQ(33, msg.nonce());
    EXPECT_EQ(51000000000, msg.gas_price());
    EXPECT_EQ(21000, msg.start_gas());
    EXPECT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    EXPECT_EQ(2600000000000000, msg.value());
    EXPECT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    EXPECT_EQ(38, msg.v());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s());
    std::vector<uint8_t> sig_vec = sig.signature();
    EXPECT_EQ(65, sig_vec.size());
    EXPECT_EQ('-', sig_vec.at(0));
    EXPECT_EQ(0x01, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    EXPECT_EQ(44, sig_unsigned_vec.size());
    EXPECT_EQ(0xeb, sig_unsigned_vec.at(0));
    EXPECT_EQ(0x80, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
    std::vector<uint8_t> unsigned_msg = msg.get_unsigned_msg();
    utils::crypto::Sha256 msg_hash = utils::crypto::keccak_sha3(unsigned_msg.data(), 0, unsigned_msg.size());
    std::vector<uint8_t> public_key = sig.recover(msg_hash);
    ASSERT_TRUE(sig.verify(public_key, unsigned_msg));
}


TEST_F(EthTxMessageTest, test_tx_decoding_empty_data) {
    std::string rlp_str = "f8522c830493e0830186a080830f4240801ba05a370e8b7b7c9cb7353de15f20a3a27e98d55cc2459e9e40f0123d2e36a423dca02245d14fc92b226cd558a90df6bf2de81a2314c53d9d7265f2c1687892216291";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    std::string address_str;
    std::string data_str;
    EXPECT_EQ(44, msg.nonce());
    EXPECT_EQ(300000, msg.gas_price());
    EXPECT_EQ(100000, msg.start_gas());
    EXPECT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    EXPECT_EQ(1000000, msg.value());
    EXPECT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    EXPECT_EQ(27, msg.v());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s());
    std::vector<uint8_t> sig_vec = sig.signature();
    EXPECT_EQ(65, sig_vec.size());
    EXPECT_EQ('Z', sig_vec.at(0));
    EXPECT_EQ(0x00, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    EXPECT_EQ(16, sig_unsigned_vec.size());
    EXPECT_EQ(0xcf, sig_unsigned_vec.at(0));
    EXPECT_EQ(0x80, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
    std::vector<uint8_t> unsigned_msg = msg.get_unsigned_msg();
    utils::crypto::Sha256 msg_hash = utils::crypto::keccak_sha3(unsigned_msg.data(), 0, unsigned_msg.size());
    std::vector<uint8_t> public_key = sig.recover(msg_hash);
    ASSERT_TRUE(sig.verify(public_key, unsigned_msg));
}

TEST_F(EthTxMessageTest, test_tx_signature_validation) {
    std::string rlp_str = "f8508085014f46b04082cf0880808025a03b636eb9d514ee5da9693131a31d2330e2e1207db05d2a1467fbe53428d508e2a060e3f66aca929cbf75a81abbe39db923e20c203d167006b849027c4de401bd3b";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    std::string address_str;
    std::string data_str;
    EXPECT_EQ(0, msg.nonce());
    EXPECT_EQ(5625000000, msg.gas_price());
    EXPECT_EQ(53000, msg.start_gas());
    EXPECT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    EXPECT_EQ(0, msg.value());
    EXPECT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    EXPECT_EQ(37, msg.v());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s());
    std::vector<uint8_t> sig_vec = sig.signature();
    EXPECT_EQ(65, sig_vec.size());
    EXPECT_EQ(';', sig_vec.at(0));
    EXPECT_EQ(0x00, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    EXPECT_EQ(17, sig_unsigned_vec.size());
    EXPECT_EQ(0xd0, sig_unsigned_vec.at(0));
    EXPECT_EQ(0x80, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
    std::vector<uint8_t> unsigned_msg = msg.get_unsigned_msg();
    utils::crypto::Sha256 msg_hash = utils::crypto::keccak_sha3(unsigned_msg.data(), 0, unsigned_msg.size());
    std::vector<uint8_t> public_key = sig.recover(msg_hash);
    ASSERT_TRUE(sig.verify(public_key, unsigned_msg));
}
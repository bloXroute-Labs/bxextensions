#include <vector>
#include <gtest/gtest.h>
#include <utils/common/byte_array.h>
#include <utils/common/ordered_map.h>
#include <utils/protocols/ethereum/eth_tx_message.h>
#include <utils/protocols/ethereum/eth_transaction_validator.h>
#include <utils/common/string_helper.h>
#include <utils/crypto/signature.h>
#include "utils/crypto/sha256.h"

typedef utils::crypto::Sha256 Sha256_t;
typedef utils::common::ByteArray ByteArray_t;
typedef utils::common::BufferView BufferView_t;
typedef utils::protocols::ethereum::EthTxMessage EthTxMessage_t;
typedef utils::protocols::ethereum::EthTransactionValidator EthTransactionValidator_t;

typedef std::pair<double, uint64_t> SenderNonceVal_t;
typedef utils::crypto::Sha256OrderedMap_t<SenderNonceVal_t> SenderNonceMap_t;
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
    std::vector<uint64_t> expected_values(1,2600000000000000);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        EXPECT_EQ(expected_values[i], values[i]);
    }
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
    std::vector<uint64_t> expected_values(1, 1000000);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        EXPECT_EQ(expected_values[i], values[i]);
    }
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
    std::vector<uint64_t> expected_values(1, 0);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        EXPECT_EQ(expected_values[i], values[i]);
    }
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

TEST_F(EthTxMessageTest, test_tx_validtor) {
    EthTransactionValidator_t eth_validator;
    SenderNonceMap_t sender_nonce_map;
    Sha256_t result;

    std::string rlp_str = "f8641f851faa3b50008255f0940000000000000000000000000000000000000000808026a0820be10a5356d8f020026540fd4c74c5e5ccd8d48f5ef1a71e3564c41f8e593aa0133440688c6c810954225367c9e7bb4d267cf8d15cb0249d4c3660541024e95f";
    std::vector<uint8_t> rlp_vec;
    EthTxMessage_t tx_msg;

    utils::common::from_hex_string(rlp_str, rlp_vec);
    BufferView_t bf(&rlp_vec.at(0), rlp_vec.size());

    eth_validator._parse_transaction(bf, tx_msg);
    eth_validator._verify_transaction_signature(tx_msg, result);
    eth_validator.transaction_validation(
        bf,
        0,
        0.1,
        sender_nonce_map,
        2,
        1.1
    );
    ASSERT_EQ("000000001f00000000000000bbdef5f330f08afd93a7696f4ea79af4a41d0f80", result.hex_string());

    std::string rlp_str2 = "f8641f851fe5d61a008255f0940000000000000000000000000000000000000000808025a044fe3a9d69cf9c0c55ff8af8867c6e4186c94613e26b8bce1526778088546abda05c37f4d0076b04fef5ac3544ee902cfff1fb17120a667b45ab84587e51105088";
    std::vector<uint8_t> rlp_vec2;
    EthTxMessage_t tx_msg2;

    utils::common::from_hex_string(rlp_str2, rlp_vec2);
    BufferView_t bf2(&rlp_vec2.at(0), rlp_vec2.size());

    eth_validator._parse_transaction(bf2, tx_msg2);
    eth_validator._verify_transaction_signature(tx_msg2, result);
    ASSERT_EQ("000000001f00000000000000bbdef5f330f08afd93a7696f4ea79af4a41d0f80", result.hex_string());

    eth_validator.transaction_validation(
        bf,
        0,
        0.2,
        sender_nonce_map,
        2,
        1.1
    );

}
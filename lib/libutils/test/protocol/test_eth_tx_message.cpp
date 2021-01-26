#include <vector>
#include <gtest/gtest.h>
#include <utils/common/byte_array.h>
#include <utils/common/ordered_map.h>
#include <utils/protocols/ethereum/eth_tx_message.h>
#include <utils/protocols/ethereum/eth_transaction_validator.h>
#include <utils/common/string_helper.h>
#include <utils/crypto/signature.h>

typedef utils::common::ByteArray ByteArray_t;
typedef utils::common::BufferView BufferView_t;
typedef utils::protocols::ethereum::EthTxMessage EthTxMessage_t;
typedef utils::protocols::ethereum::EthTransactionValidator EthTransactionValidator_t;

typedef std::pair<double, uint64_t> SenderNonceVal_t;
typedef utils::common::OrderedMap<std::string, SenderNonceVal_t> SenderNonceMap_t;
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
    std::string rlp_str = "f86682032a850a02ffee008255f0942e059e37367ba9f66a0f75ad17f3a9eb7ed8d063808026a0e3186d7f0954318d84d97d4065cbbd5546bc3aee21665869b8230960efca9bf5a034f6563ec353a111fb3ac14797d0b31a3b9f7ba0f9df139d91d8477a0d5e89cb";
    std::vector<uint8_t> rlp_vec;
    EthTransactionValidator_t eth_validator;
    SenderNonceMap_t sender_nonce_map;

    utils::common::from_hex_string(rlp_str, rlp_vec);
    BufferView_t bf(&rlp_vec.at(0), rlp_vec.size());

    eth_validator.transaction_validation(
        bf,
        0,
        0.1,
        sender_nonce_map,
        2,
        1.1
    );

    rlp_str = "f9015582da98850fd51da800830288ac94d9e1ce17f2641f24ae83637ab66a2cca9c378b9f8901a055690d9db80000b8e47ff36ab5000000000000000000000000000000000000000000000237027f7cc2ef1ba38f00000000000000000000000000000000000000000000000000000000000000800000000000000000000000000dc411b17d337af85d83ea5a3577d09132aae866000000000000000000000000000000000000000000000000000000005fb26cb70000000000000000000000000000000000000000000000000000000000000002000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000006b3595068778dd592e39a122f4f5a5cf09c90fe225a01945291e6d750eb41d07b74838a601aa32334898c8da5b731960108a7db54beca05b48c4c31f5836b3fb35194b8d73bf907d795cd6bed65dc6c57ed1edc35dd3f7";
    utils::common::from_hex_string(rlp_str, rlp_vec);
    bf = BufferView_t(&rlp_vec.at(0), rlp_vec.size());

    eth_validator.transaction_validation(
        bf,
        0,
        0.2,
        sender_nonce_map,
        2,
        1.1
    );

}
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
typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;
typedef utils::protocols::ethereum::EthTxMessage EthTxMessage_t;
typedef utils::protocols::ethereum::EthTransactionValidator EthTransactionValidator_t;

typedef std::pair<double, uint64_t> SenderNonceVal_t;
typedef utils::crypto::Sha256OrderedMap_t<SenderNonceVal_t> SenderNonceMap_t;
class EthTxMessageTest : public ::testing::Test {
};

TEST_F(EthTxMessageTest, test_tx_decoding_legacy) {
    std::string rlp_str = "f86b21850bdfd63e0082520894f8044ff824c2dce174b4ee9f958c2a738483189e87093cafac6a80008026a02dbf2ca92baeab4a03cdfae33cbf240065e24e7cc9f7e2a99c3edf6e0c4fc016a029114b3d3b96587d61d5000665537ad12ce43ea18cf87f3e0e3ad1cd003f2715";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    std::string address_str = "f8044ff824c2dce174b4ee9f958c2a738483189e";
    std::string data_str;
    ASSERT_EQ(33, msg.nonce());
    ASSERT_EQ(51000000000, msg.gas_price());
    ASSERT_EQ(21000, msg.gas_limit());
    ASSERT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    std::vector<uint64_t> expected_values(1,2600000000000000);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_EQ(expected_values[i], values[i]);
    }
    ASSERT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    ASSERT_EQ(38, msg.v());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s(), msg.payload_type(), msg.y_parity());
    std::vector<uint8_t> sig_vec = sig.signature();
    ASSERT_EQ(65, sig_vec.size());
    ASSERT_EQ('-', sig_vec.at(0));
    ASSERT_EQ(0x01, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    ASSERT_EQ(44, sig_unsigned_vec.size());
    ASSERT_EQ(0xeb, sig_unsigned_vec.at(0));
    ASSERT_EQ(0x80, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
    utils::crypto::Sha256 msg_hash = utils::crypto::keccak_sha3(sig_unsigned_vec.data(), 0, sig_unsigned_vec.size());
    std::vector<uint8_t> public_key = sig.recover(msg_hash);
    ASSERT_TRUE(sig.verify(public_key, sig_unsigned_vec));
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
    ASSERT_EQ(44, msg.nonce());
    ASSERT_EQ(300000, msg.gas_price());
    ASSERT_EQ(100000, msg.gas_limit());
    ASSERT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    std::vector<uint64_t> expected_values(1, 1000000);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_EQ(expected_values[i], values[i]);
    }
    ASSERT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    ASSERT_EQ(27, msg.v());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s(), msg.payload_type(), msg.y_parity());
    std::vector<uint8_t> sig_vec = sig.signature();
    ASSERT_EQ(65, sig_vec.size());
    ASSERT_EQ('Z', sig_vec.at(0));
    ASSERT_EQ(0x00, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    ASSERT_EQ(16, sig_unsigned_vec.size());
    ASSERT_EQ(0xcf, sig_unsigned_vec.at(0));
    ASSERT_EQ(0x80, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
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
    ASSERT_EQ(0, msg.nonce());
    ASSERT_EQ(5625000000, msg.gas_price());
    ASSERT_EQ(53000, msg.gas_limit());
    ASSERT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    std::vector<uint64_t> expected_values(1, 0);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_EQ(expected_values[i], values[i]);
    }
    ASSERT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    ASSERT_EQ(37, msg.v());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s(), msg.payload_type(), msg.y_parity());
    std::vector<uint8_t> sig_vec = sig.signature();
    ASSERT_EQ(65, sig_vec.size());
    ASSERT_EQ(';', sig_vec.at(0));
    ASSERT_EQ(0x00, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    ASSERT_EQ(17, sig_unsigned_vec.size());
    ASSERT_EQ(0xd0, sig_unsigned_vec.at(0));
    ASSERT_EQ(0x80, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
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

    eth_validator._parse_transaction(std::make_shared<TxContents_t>(bf), tx_msg);
    eth_validator._verify_transaction_signature(tx_msg, result);
    eth_validator.transaction_validation(
        std::make_shared<TxContents_t>(bf),
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

    eth_validator._parse_transaction(std::make_shared<TxContents_t>(bf2), tx_msg2);
    eth_validator._verify_transaction_signature(tx_msg2, result);
    ASSERT_EQ("000000001f00000000000000bbdef5f330f08afd93a7696f4ea79af4a41d0f80", result.hex_string());

    eth_validator.transaction_validation(
        std::make_shared<TxContents_t>(bf2),
        0,
        0.2,
        sender_nonce_map,
        2,
        1.1
    );
}

TEST_F(EthTxMessageTest, test_tx_decoding_type_1) {
    std::string rlp_str = "b9022401f9022101829237853486ced000830285ee94653911da49db4cdb0b7c3e4d929cfb56024cd4e680b8a48201aa3f000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000000000000000000000000000000000000000000000083a297567e20f8000000000000000000000000000d8775f648430679a709e98d2b0cb6250d2887ef000000000000000000000000000000000000000000000358c5ee87d374000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff90111f859940d8775f648430679a709e98d2b0cb6250d2887eff842a01d76467e21923adb4ee07bcae017030c6208bbccde21ff0a61518956ad9b152aa0ec5bfdd140da829800c64d740e802727fca06fadec8b5d82a7b406c811851b55f85994653911da49db4cdb0b7c3e4d929cfb56024cd4e6f842a02a9a57a342e03a2b55a8bef24e9c777df22a7442475b1641875a66dba65855f0a0d0bcf4df132c65dad73803c5e5e1c826f151a3342680034a8a4c8e5f8eb0c13ff85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a01fc85b67921559ce4fef22a331ff00c886678cf8b163d395e45fe0543f8750bda047a365b3ae9dbfa1c60a2cd30347765e914a89b7dac828db3ac3bd3e775b1a9980a0a340fc367050387a1b295514210a48de3836ee7923d9739bf0104e6d79c37997a06e63c7801da3c72f1a53f9b809ae04a45637da673c2a9c47065a1b1cdeafef7d";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);

    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    ASSERT_EQ("9310dc4f07748222d37f43c7296826cf4bf6693fa207968bd7500659ee2cc04d", msg.hash().hex_string() );
    ASSERT_EQ(37431, msg.nonce());
    ASSERT_EQ(225600000000, msg.gas_price());
    ASSERT_EQ(165358, msg.gas_limit());
    ASSERT_EQ("653911da49db4cdb0b7c3e4d929cfb56024cd4e6", utils::common::to_hex_string(msg.address()));
    std::vector<uint64_t> expected_values(1,0000000000000000);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_EQ(expected_values[i], values[i]);
    }
    ASSERT_EQ(0, msg.v());
    ASSERT_EQ(0, msg.y_parity());
}

TEST_F(EthTxMessageTest, test_tx_signature_validation_type_1) {
    std::string rlp_str = "b9022401f9022101829237853486ced000830285ee94653911da49db4cdb0b7c3e4d929cfb56024cd4e680b8a48201aa3f000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000000000000000000000000000000000000000000000083a297567e20f8000000000000000000000000000d8775f648430679a709e98d2b0cb6250d2887ef000000000000000000000000000000000000000000000358c5ee87d374000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff90111f859940d8775f648430679a709e98d2b0cb6250d2887eff842a01d76467e21923adb4ee07bcae017030c6208bbccde21ff0a61518956ad9b152aa0ec5bfdd140da829800c64d740e802727fca06fadec8b5d82a7b406c811851b55f85994653911da49db4cdb0b7c3e4d929cfb56024cd4e6f842a02a9a57a342e03a2b55a8bef24e9c777df22a7442475b1641875a66dba65855f0a0d0bcf4df132c65dad73803c5e5e1c826f151a3342680034a8a4c8e5f8eb0c13ff85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a01fc85b67921559ce4fef22a331ff00c886678cf8b163d395e45fe0543f8750bda047a365b3ae9dbfa1c60a2cd30347765e914a89b7dac828db3ac3bd3e775b1a9980a0a340fc367050387a1b295514210a48de3836ee7923d9739bf0104e6d79c37997a06e63c7801da3c72f1a53f9b809ae04a45637da673c2a9c47065a1b1cdeafef7d";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());

    utils::crypto::Signature sig;
    sig.encode_signature(msg.v(), msg.r(), msg.s(), msg.payload_type(), msg.y_parity());
    std::vector<uint8_t> sig_vec = sig.signature();
    ASSERT_EQ(65, sig_vec.size());
    ASSERT_EQ(163, sig_vec.at(0));
    ASSERT_EQ(0x00, sig_vec.at(sig_vec.size() - 1));
    std::vector<uint8_t> sig_unsigned_vec = msg.get_unsigned_msg();
    ASSERT_EQ(482, sig_unsigned_vec.size());
    ASSERT_EQ(0x01, sig_unsigned_vec.at(0));
    ASSERT_EQ(0x99, sig_unsigned_vec.at(sig_unsigned_vec.size() - 1));
    utils::crypto::Sha256 msg_hash = utils::crypto::keccak_sha3(sig_unsigned_vec.data(), 0, sig_unsigned_vec.size());
    std::vector<uint8_t> public_key = sig.recover(msg_hash);
    ASSERT_TRUE(sig.verify(public_key, sig_unsigned_vec));
}

TEST_F(EthTxMessageTest, test_tx_validtor_type_1) {
    EthTransactionValidator_t eth_validator;
    SenderNonceMap_t sender_nonce_map;
    Sha256_t result;

    std::string rlp_str = "b9022401f9022101829237853486ced000830285ee94653911da49db4cdb0b7c3e4d929cfb56024cd4e680b8a48201aa3f000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000000000000000000000000000000000000000000000083a297567e20f8000000000000000000000000000d8775f648430679a709e98d2b0cb6250d2887ef000000000000000000000000000000000000000000000358c5ee87d374000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff90111f859940d8775f648430679a709e98d2b0cb6250d2887eff842a01d76467e21923adb4ee07bcae017030c6208bbccde21ff0a61518956ad9b152aa0ec5bfdd140da829800c64d740e802727fca06fadec8b5d82a7b406c811851b55f85994653911da49db4cdb0b7c3e4d929cfb56024cd4e6f842a02a9a57a342e03a2b55a8bef24e9c777df22a7442475b1641875a66dba65855f0a0d0bcf4df132c65dad73803c5e5e1c826f151a3342680034a8a4c8e5f8eb0c13ff85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a01fc85b67921559ce4fef22a331ff00c886678cf8b163d395e45fe0543f8750bda047a365b3ae9dbfa1c60a2cd30347765e914a89b7dac828db3ac3bd3e775b1a9980a0a340fc367050387a1b295514210a48de3836ee7923d9739bf0104e6d79c37997a06e63c7801da3c72f1a53f9b809ae04a45637da673c2a9c47065a1b1cdeafef7d";
    std::vector<uint8_t> rlp_vec;
    EthTxMessage_t tx_msg;

    utils::common::from_hex_string(rlp_str, rlp_vec);
    BufferView_t bf(&rlp_vec.at(0), rlp_vec.size());

    eth_validator._parse_transaction(std::make_shared<TxContents_t>(bf), tx_msg);
    eth_validator._verify_transaction_signature(tx_msg, result);
    ASSERT_EQ("0000000037920000000000000087c5900b9bbc051b5f6299f5bce92383273b28", result.hex_string());
}

TEST_F(EthTxMessageTest, test_tx_validtor_type_1_random_tx) {
    EthTransactionValidator_t eth_validator;
    SenderNonceMap_t sender_nonce_map;
    Sha256_t result;

    std::string rlp_str = "b9022501f9022101827fa38538702a780083029c3294bed340a301b4f07fa7b61ab9e0767faa172f530d80b8a48201aa3f000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2000000000000000000000000000000000000000000000000b100e0916e8290000000000000000000000000001f9840a85d5af5bf1d1762f925bdaddc4201f984000000000000000000000000000000000000000000000030383535d293160000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff90111f859941f9840a85d5af5bf1d1762f925bdaddc4201f984f842a077c21c217c9720c8511a68d2827358c857fde45df5e9ca5e32c8c36391a55fdca0852d14247ed022af8681a666e93f715eef53196b26b1c55f800ff8c53a7af1dbf85994bed340a301b4f07fa7b61ab9e0767faa172f530df842a029b1a034bb2081c2fc39d175406b4ff0e9b9b4a6008604cc27c4a43c257539d8a0d0bcf4df132c65dad73803c5e5e1c826f151a3342680034a8a4c8e5f8eb0c13ff85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a0a4b587f9a312bb1e2933f16f83d660ce4065c5ca4acb09859f2fcf70136831eaa0c209eb352a72bc830d8ff6e3bb34742d861e27a1111c497a0a45515cdbaecc5101a0373fa514073f5580fe27ac635693741b931a7d14e8cf0e0f176a5765a3e0f49aa05de7d07fe0cd2a6e848af4d8d03bd9b8cfbc5736407a0c69e565fe716d0afd16";
    std::vector<uint8_t> rlp_vec;
    EthTxMessage_t tx_msg;

    utils::common::from_hex_string(rlp_str, rlp_vec);
    BufferView_t bf(&rlp_vec.at(0), rlp_vec.size());

    eth_validator._parse_transaction(std::make_shared<TxContents_t>(bf), tx_msg);
    ASSERT_TRUE(eth_validator._verify_transaction_signature(tx_msg, result));
}

TEST_F(EthTxMessageTest, tx_jo_json_legacy) {
    std::string rlp_str = "f86b21850bdfd63e0082520894f8044ff824c2dce174b4ee9f958c2a738483189e87093cafac6a80008026a02dbf2ca92baeab4a03cdfae33cbf240065e24e7cc9f7e2a99c3edf6e0c4fc016a029114b3d3b96587d61d5000665537ad12ce43ea18cf87f3e0e3ad1cd003f2715";
    std::string expected_json = "{\"type\":\"0x00\",\"hash\":\"0x397c39244abc618a37a03044eeb3d247e1335cb1263aa8cf5f0afdd8942e5e11\",\"nonce\":\"0x21\",\"gasPrice\":\"0xbdfd63e00\",\"gas\":\"0x5208\",\"to\":\"0xf8044ff824c2dce174b4ee9f958c2a738483189e\",\"value\":\"0x00\",\"data\":\"0x\",\"v\":\"0x26\",\"r\":\"0x2dbf2ca92baeab4a03cdfae33cbf240065e24e7cc9f7e2a99c3edf6e0c4fc016\",\"s\":\"0x29114b3d3b96587d61d5000665537ad12ce43ea18cf87f3e0e3ad1cd003f2715\"}";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    std::string address_str = "f8044ff824c2dce174b4ee9f958c2a738483189e";
    std::string data_str;
    ASSERT_EQ(33, msg.nonce());
    ASSERT_EQ(51000000000, msg.gas_price());
    ASSERT_EQ(21000, msg.gas_limit());
    ASSERT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    std::vector<uint64_t> expected_values(1,2600000000000000);
    std::vector<uint64_t> values = msg.value();
    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_EQ(expected_values[i], values[i]);
    }
    ASSERT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    ASSERT_EQ(38, msg.v());

    std::string json = msg.to_json();
    ASSERT_EQ(expected_json, json);
}

TEST_F(EthTxMessageTest, tx_jo_json_type2) {
    std::string rlp_str = "b902a102f9029d0182184285093881a7eb85093881a7eb8307a12094000000005736775feb0c8568e7dee77222a2688080b8540000000100c738dac02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000002000000000000000029e5c8906e7193c000000000000000a0a3f8206412841f56f1a91c7d44768070f711c68f33a7ca25c8d30268f901d7f85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a0fe1528ce3c182d2d7eed0b70b1a8cfc18405d17870fc7b7cb94a3cdb7e145d87a0b49bf121c8665d4fea844602438b390ab1200890f40ffdf0d7af651c1b5ea755f8dd94984a3eab3cf2fc2b4ca6e4a3768624a8272fe2a3f8c6a0000000000000000000000000000000000000000000000000000000000000000ca00000000000000000000000000000000000000000000000000000000000000008a00000000000000000000000000000000000000000000000000000000000000006a00000000000000000000000000000000000000000000000000000000000000007a00000000000000000000000000000000000000000000000000000000000000009a0000000000000000000000000000000000000000000000000000000000000000af89b94f1a91c7d44768070f711c68f33a7ca25c8d30268f884a07e8f21cb3285970cbc690329c49b32fe49503511c52f602d3e8e2a1182753870a00000000000000000000000000000000000000000000000000000000000000007a00000000000000000000000000000000000000000000000000000000000000008a0675a164006bce49fe78035726b6a3c528eb9c13de53398f7659c346d8ea5c3b201a0c885b3e31c9390bd9613e72b365cb750a2d2edd66dc0cf46fb460296de54b00fa025feb091ed600c1a20f4ab0b370fb18bc6deca1f6d54a2177c80cfd53b00174a";
    std::string expected_json = "{\"type\":\"0x02\",\"hash\":\"0x85fc00ccbb990fd6e35bbbd7b21d75a84732eeb5f202f1cd14bc444375007639\",\"chainId\":\"0x01\",\"nonce\":\"0x1842\",\"maxPriorityFeePerGas\":\"0x93881a7eb\",\"maxFeePerGas\":\"0x93881a7eb\",\"startGas\":\"0x00\",\"to\":\"0x000000005736775feb0c8568e7dee77222a26880\",\"value\":\"0x00\",\"data\":\"0x0000000100c738dac02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000002000000000000000029e5c8906e7193c000000000000000a0a3f8206412841f56f1a91c7d44768070f711c68f33a7ca25c8d30268\",\"accessList\":[{\"address\":\"0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2\",\"storageKeys\":[\"0xfe1528ce3c182d2d7eed0b70b1a8cfc18405d17870fc7b7cb94a3cdb7e145d87\",\"0xb49bf121c8665d4fea844602438b390ab1200890f40ffdf0d7af651c1b5ea755\"]},{\"address\":\"0x984a3eab3cf2fc2b4ca6e4a3768624a8272fe2a3\",\"storageKeys\":[\"0x000000000000000000000000000000000000000000000000000000000000000c\",\"0x0000000000000000000000000000000000000000000000000000000000000008\",\"0x0000000000000000000000000000000000000000000000000000000000000006\",\"0x0000000000000000000000000000000000000000000000000000000000000007\",\"0x0000000000000000000000000000000000000000000000000000000000000009\",\"0x000000000000000000000000000000000000000000000000000000000000000a\"]},{\"address\":\"0xf1a91c7d44768070f711c68f33a7ca25c8d30268\",\"storageKeys\":[\"0x7e8f21cb3285970cbc690329c49b32fe49503511c52f602d3e8e2a1182753870\",\"0x0000000000000000000000000000000000000000000000000000000000000007\",\"0x0000000000000000000000000000000000000000000000000000000000000008\",\"0x675a164006bce49fe78035726b6a3c528eb9c13de53398f7659c346d8ea5c3b2\"]}]\"v\":\"0x01\",\"r\":\"0xc885b3e31c9390bd9613e72b365cb750a2d2edd66dc0cf46fb460296de54b00f\",\"s\":\"0x25feb091ed600c1a20f4ab0b370fb18bc6deca1f6d54a2177c80cfd53b00174a\"}";
    std::vector<uint8_t > rlp_vec;
    utils::common::from_hex_string(rlp_str, rlp_vec);
    EthTxMessage_t msg;
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    std::string address_str = "000000005736775feb0c8568e7dee77222a26880";
    std::string data_str;
    ASSERT_EQ("85fc00ccbb990fd6e35bbbd7b21d75a84732eeb5f202f1cd14bc444375007639", msg.hash().hex_string());
    ASSERT_EQ(2, msg.payload_type());
    ASSERT_EQ(1, msg.chain_id());
    ASSERT_EQ(0x1842, msg.nonce());
    ASSERT_EQ(0x93881a7eb, msg.max_priority_fee_per_gas());
    ASSERT_EQ(0x93881a7eb, msg.max_fee_per_gas());
    ASSERT_EQ(address_str, utils::common::to_hex_string(msg.address()));
//    std::vector<uint64_t> expected_values(1,2600000000000000);
//    std::vector<uint64_t> values = msg.value();
//    for (size_t i = 0; i < values.size(); i++) {
//        ASSERT_EQ(expected_values[i], values[i]);
//    }
//    ASSERT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    ASSERT_EQ(0, msg.v());

    std::string json = msg.to_json();
    ASSERT_EQ(expected_json, json);

    rlp_str = "02f9029d0182184285093881a7eb85093881a7eb8307a12094000000005736775feb0c8568e7dee77222a2688080b8540000000100c738dac02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000002000000000000000029e5c8906e7193c000000000000000a0a3f8206412841f56f1a91c7d44768070f711c68f33a7ca25c8d30268f901d7f85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a0fe1528ce3c182d2d7eed0b70b1a8cfc18405d17870fc7b7cb94a3cdb7e145d87a0b49bf121c8665d4fea844602438b390ab1200890f40ffdf0d7af651c1b5ea755f8dd94984a3eab3cf2fc2b4ca6e4a3768624a8272fe2a3f8c6a0000000000000000000000000000000000000000000000000000000000000000ca00000000000000000000000000000000000000000000000000000000000000008a00000000000000000000000000000000000000000000000000000000000000006a00000000000000000000000000000000000000000000000000000000000000007a00000000000000000000000000000000000000000000000000000000000000009a0000000000000000000000000000000000000000000000000000000000000000af89b94f1a91c7d44768070f711c68f33a7ca25c8d30268f884a07e8f21cb3285970cbc690329c49b32fe49503511c52f602d3e8e2a1182753870a00000000000000000000000000000000000000000000000000000000000000007a00000000000000000000000000000000000000000000000000000000000000008a0675a164006bce49fe78035726b6a3c528eb9c13de53398f7659c346d8ea5c3b201a0c885b3e31c9390bd9613e72b365cb750a2d2edd66dc0cf46fb460296de54b00fa025feb091ed600c1a20f4ab0b370fb18bc6deca1f6d54a2177c80cfd53b00174a";
    rlp_vec.clear();
    utils::common::from_hex_string(rlp_str, rlp_vec);
    msg.decode(BufferView_t(&rlp_vec.at(0), rlp_vec.size()), 0);
    ASSERT_TRUE(msg.deserialize());
    ASSERT_EQ("85fc00ccbb990fd6e35bbbd7b21d75a84732eeb5f202f1cd14bc444375007639", msg.hash().hex_string());
    ASSERT_EQ(2, msg.payload_type());
    ASSERT_EQ(1, msg.chain_id());
    ASSERT_EQ(0x1842, msg.nonce());
    ASSERT_EQ(0x93881a7eb, msg.max_priority_fee_per_gas());
    ASSERT_EQ(0x93881a7eb, msg.max_fee_per_gas());
    ASSERT_EQ(address_str, utils::common::to_hex_string(msg.address()));
    //    std::vector<uint64_t> expected_values(1,2600000000000000);
    //    std::vector<uint64_t> values = msg.value();
    //    for (size_t i = 0; i < values.size(); i++) {
    //        ASSERT_EQ(expected_values[i], values[i]);
    //    }
    //    ASSERT_EQ(data_str, utils::common::to_hex_string(msg.data()));
    ASSERT_EQ(0, msg.v());

    json = msg.to_json();
    ASSERT_EQ(expected_json, json);
}

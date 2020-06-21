//#include <gtest/gtest.h>
//#include <iostream>     // std::cout
//#include <fstream>      // std::ifstream
//
//#include "utils/protocols/ethereum/eth_block_message.h"
//#include <utils/common/byte_array.h>
//#include "utils/common/string_helper.h"
//#include "utils/common/buffer_view.h"
//#include "utils/crypto/hash_helper.h"
//
//#include "tpe/service/transaction_service.h"
//
//#include "tpe/task/eth_block_compression_task.h"
//#include "tpe/task_pool_executor.h"
//
//typedef utils::common::BufferView BufferView_t;
//typedef utils::common::ByteArray ByteArray_t;
//typedef std::shared_ptr<std::vector<uint8_t>> PVec_t;
//
//
//class EthBlockCompressionTaskTest : public ::testing::Test {
//public:
//    PVec_t hex_string_to_vec(std::string& hex_string);
//    utils::protocols::ethereum::EthBlockMessage create_eth_block_msg();
//};
//
//PVec_t EthBlockCompressionTaskTest::hex_string_to_vec(std::string& hex_string)
//{
//    std::vector<uint8_t> vec;
//    utils::common::from_hex_string(hex_string, vec);
//    return std::move(std::make_shared<std::vector<uint8_t>>(vec));
//}
//
//utils::protocols::ethereum::EthBlockMessage EthBlockCompressionTaskTest::create_eth_block_msg(){
//    PVec_t vec(nullptr);
//    std::ifstream eth_block_sample_file("/Users/daniellemachpud/workspace/bloxroute-dev/bxextensions/lib/libtpe/test/data/eth_sample_block");
//    std::string block_message_hex_string(
//        std::istreambuf_iterator<char>(eth_block_sample_file),
//        (std::istreambuf_iterator<char>())
//    );
//
//    vec = std::move(hex_string_to_vec(block_message_hex_string));
//    BufferView_t block_message(*vec);
//    return utils::protocols::ethereum::EthBlockMessage(block_message);
//}
//
//
//TEST_F(EthBlockCompressionTaskTest, test_eth_block_compression)
//{
//    utils::protocols::ethereum::EthBlockMessage eth_block_msg = create_eth_block_msg();
//    eth_block_msg.parse();
//    std::string prev_block_hash_str = eth_block_msg.prev_block_hash().hex_string();
//    std::cout << "block header: " << utils::common::to_hex_string(eth_block_msg.block_header()) << std::endl;
//    std::cout << "header size: " << eth_block_msg.block_header().size() << std::endl;
////    ASSERT_EQ(prev_block_hash_str, "cdd1e2a696eb4c62c3a107c3a64098bad2987084d303d20ef2ad8d4307a4544d");
////    ASSERT_EQ(15686, eth_block_msg.txn_end_offset() - eth_block_msg.txn_offset());
//}
//

#include <gtest/gtest.h>
#include <iostream>     // std::cout

#include "utils/protocols/ethereum/eth_block_message.h"
#include <utils/common/byte_array.h>
#include "utils/common/string_helper.h"
#include "utils/common/buffer_view.h"
#include "utils/crypto/hash_helper.h"

#include "tpe/service/transaction_service.h"

#include "tpe/task/eth_block_compression_task.h"
#include "tpe/task_pool_executor.h"

typedef utils::common::BufferView BufferView_t;
typedef utils::common::BufferCopy BufferCopy_t;
typedef std::shared_ptr<std::vector<uint8_t>> PVec_t;


class EthBlockCompressionTaskTest : public ::testing::Test {
public:
    BufferCopy_t convert_str_to_buffer(std::string block_msg_str);
};

BufferCopy_t EthBlockCompressionTaskTest::convert_str_to_buffer(std::string block_msg_str)
{
    std::vector<uint8_t> block_msg_vec;
    utils::common::from_hex_string(block_msg_str, block_msg_vec);
    BufferCopy_t block_message(BufferView_t(&block_msg_vec.at(0), block_msg_vec.size()));
    return block_message;
}


TEST_F(EthBlockCompressionTaskTest, test_eth_block_parsing)
{
    std::string block_msg_str = "f909b6f9024aa067bef956bc8fab2d2ea392795aacd9f9f981e4bb57e7662bea88f8e1d6159a51a0cdea682c43f9c5e2dfa359a815651967f8b8bd8d630c1e5efda718a500d2361c9426dd57d260f91f9dea44662ba7a8f20c3e59a642a02e618bd9ae364db3c3c2a8712ee49c0bcf1e9c0caaa624ee67145eb6781c5623a06fef5ee4713566107de234216c1148f3c96f3b93f7213319c4c1fd12241a04d5a0d123e1dbff5575789b01f9b4a3b00fbe9c3b7e28ea07cec111e18ee2f65ff476b90100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000640102030405b8645f9cd87e48848bca610cfe4166a3516f407f0a913fea6863f16eb8bd022ba24e737ddce176b53a27cd6e685b2fcffb7d4a2f718f8c1bf99056ced62198b6245019bd809bd1a4dc70dfffca71a2550e5c9ce62520ae47d91c5ab30368d726050ded4a1c51a0848851494cd3676eb9e602767ab50840bd90c86e26c5be506e3968dbb2698bd91df90195ec0102039460116761d985d2096f6e3a0c4b45ff0db9b21534048f2b9681da17cddc3fc341122599ddc91b0607ec01020394e7b3f8c870315dab969afa6b2ae271da7519e0bc048f54a52696b3c00125828cd8e4a8d6831b0607ec010203942f5a7e4d41c88448b5bf413653268e83043b35a4048fe87f84ac5b392dd481510d4b551ebe1b0607ec01020394a71dfe590a84b53af2a05ca0388f740d2a807300048fcf91705313f528bc31757684ccd55c1b0607ec01020394cce69bbc9af6ec6a11879662a3f555dad4f473e8048f3c9285b81c676b50fd7bd54da80e9c1b0607ec01020394e7efb25b2f50c100953153e9a93fc85c69863049048f481a94422cb6395ee10fec8c8aef181b0607ec0102039409af4295917ef873880a53477cf7dd30e62dfe4c048f957b2e3ab1bf022d95ed2f7258c6c61b0607ec01020394d6c095646f16295580ada2d93c37b1b33ec8e1ba048f472cbc2d7c4ed8f3b9aec0e1bc02811b0607ec0102039405d4331183d6e25dc28addbbf4a6b3a5c7a22d2e048fe9ecfbf27e23496466cd03932c05541b0607f905ccf902b0a08ad987a6f1d3901b28c5d30fb0b2caea0237b67fe517641a7b58fd246d1e2c4da0c35900f5070f8ae815bb77781ee808c9831daf109ee3a8bdcea14a186068f39c94220e678fe9bbff94ce8cdfc1011dd5f9a0d95590a0b3a7573fdffffa7d74c301e68533c494c28b53767c03d74cb8f8503024caff11a08f058161af93ae08b3d629f1ee2c8d9e5696fdb17f3d14ab2149e0075a122c31a07f5a24796d690341af2e25c33ad76ad50f6e036a2061fad8ad634802f6b441b7b90100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c8020406080ab8c8b7ddadaadc278759832251f5fedf03381eea00f6d5aa2cd0cee013feb54cf27d49855cb59e30066f96d3c0f0d215d3217254703e43f77c47135a956a56990fd56ccbc5710368595ac5a1bd35f567bc82b69135113dd271afb19460ef2912969de2da63a10a6017440c5f9630c9b0fd8911d442a66a925bcb5e452e707d7a8e6f81d55867af4747487d9d33d84ff979e4e6e051c21bc8e2fa94135e5184b0bfbb025fd23adfb639c9a20327bf31f7f2de21a00a4231ef46cd12838d9495c8bc0572322447e629d3a6a0a1526d3b46a26c17e7555e0ae6d4bf20b537bf0db30c41c3cc35ab8318e0c7418269a5f90316a09877c22cb45254f2ae65c5f41d00427befa2dd6563af3900a7eda6df98dc2479a06efeefe9271e10a94852606a425140ba25f24d94bc50dca72a5ae1df402a9783948f4694c2db21c8b2ef14b02de7f82b6675913754a0b9ffbe72a71834fa1c871666942791433b9cb2e8b1764f58c626e4196fbd3516a0eb9dfb6dace81d5df786f72bcd5f749f6a1c195ee96be444185f0363b3c6ddfba0424ba61dd5ee26ee330cfb0063cdc1f2c87124d2f6d77a7817eefb1acb3dc6d5b901000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000012c0306090c0fb9012c1c3bbe2f58cdcec1bbad5822d2423a6fe2c8a5e788a3cf72bbe5e5654ce15b8169e3ebdb94cb3a5ee3027d8b148ea0669fd67cfd44437bf6a6f88485f82dab04875bd7f2afed865d4e56b246f19dea76728a4946ec195d2fc82db25d1b169be625335cd49c312dadc2188887591d055eacd2218816d6e0eb568ce5a6a4d8ae8484d8b9a4598c681d677512bb1ce3f93529008170ff92911286c157c9403f3270424fd16aba99abf4b7b7b127c2587a0d3202442ab0d5b41aa065abcc771121f7605494f11278273ac1344ddbf3b91f51863ed9a95599ddbf068b65e6eaa077414cedfa110f5d50faabd6731ab7e80f087b9b77b5f8829610242e2afca6f4b2b39971f626320cbdab9699d6ad86fe0089034d691cbe99beb1376c2479cc347a681854ea162b7ce58547a5c259a048a5d9f334d31907b01d629ab2be923bdb63e46e7e6055a042bf3545fd5b3919836e283e0a02";

    BufferCopy_t block_msg_bf = convert_str_to_buffer(block_msg_str);

    utils::protocols::ethereum::EthBlockMessage eth_block_msg(block_msg_bf);
    eth_block_msg.parse();
    EXPECT_EQ(eth_block_msg.prev_block_hash().hex_string(), "67bef956bc8fab2d2ea392795aacd9f9f981e4bb57e7662bea88f8e1d6159a51");
    EXPECT_EQ(eth_block_msg.block_header().size(), 589);
    EXPECT_EQ(utils::common::to_hex_string(eth_block_msg.block_header()), "f9024aa067bef956bc8fab2d2ea392795aacd9f9f981e4bb57e7662bea88f8e1d6159a51a0cdea682c43f9c5e2dfa359a815651967f8b8bd8d630c1e5efda718a500d2361c9426dd57d260f91f9dea44662ba7a8f20c3e59a642a02e618bd9ae364db3c3c2a8712ee49c0bcf1e9c0caaa624ee67145eb6781c5623a06fef5ee4713566107de234216c1148f3c96f3b93f7213319c4c1fd12241a04d5a0d123e1dbff5575789b01f9b4a3b00fbe9c3b7e28ea07cec111e18ee2f65ff476b90100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000640102030405b8645f9cd87e48848bca610cfe4166a3516f407f0a913fea6863f16eb8bd022ba24e737ddce176b53a27cd6e685b2fcffb7d4a2f718f8c1bf99056ced62198b6245019bd809bd1a4dc70dfffca71a2550e5c9ce62520ae47d91c5ab30368d726050ded4a1c51a0848851494cd3676eb9e602767ab50840bd90c86e26c5be506e3968dbb2698bd91d");
    EXPECT_EQ(eth_block_msg.block_trailer().size(), 1489);
    ASSERT_EQ(utils::common::to_hex_string(eth_block_msg.block_trailer()), "f905ccf902b0a08ad987a6f1d3901b28c5d30fb0b2caea0237b67fe517641a7b58fd246d1e2c4da0c35900f5070f8ae815bb77781ee808c9831daf109ee3a8bdcea14a186068f39c94220e678fe9bbff94ce8cdfc1011dd5f9a0d95590a0b3a7573fdffffa7d74c301e68533c494c28b53767c03d74cb8f8503024caff11a08f058161af93ae08b3d629f1ee2c8d9e5696fdb17f3d14ab2149e0075a122c31a07f5a24796d690341af2e25c33ad76ad50f6e036a2061fad8ad634802f6b441b7b90100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c8020406080ab8c8b7ddadaadc278759832251f5fedf03381eea00f6d5aa2cd0cee013feb54cf27d49855cb59e30066f96d3c0f0d215d3217254703e43f77c47135a956a56990fd56ccbc5710368595ac5a1bd35f567bc82b69135113dd271afb19460ef2912969de2da63a10a6017440c5f9630c9b0fd8911d442a66a925bcb5e452e707d7a8e6f81d55867af4747487d9d33d84ff979e4e6e051c21bc8e2fa94135e5184b0bfbb025fd23adfb639c9a20327bf31f7f2de21a00a4231ef46cd12838d9495c8bc0572322447e629d3a6a0a1526d3b46a26c17e7555e0ae6d4bf20b537bf0db30c41c3cc35ab8318e0c7418269a5f90316a09877c22cb45254f2ae65c5f41d00427befa2dd6563af3900a7eda6df98dc2479a06efeefe9271e10a94852606a425140ba25f24d94bc50dca72a5ae1df402a9783948f4694c2db21c8b2ef14b02de7f82b6675913754a0b9ffbe72a71834fa1c871666942791433b9cb2e8b1764f58c626e4196fbd3516a0eb9dfb6dace81d5df786f72bcd5f749f6a1c195ee96be444185f0363b3c6ddfba0424ba61dd5ee26ee330cfb0063cdc1f2c87124d2f6d77a7817eefb1acb3dc6d5b901000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000012c0306090c0fb9012c1c3bbe2f58cdcec1bbad5822d2423a6fe2c8a5e788a3cf72bbe5e5654ce15b8169e3ebdb94cb3a5ee3027d8b148ea0669fd67cfd44437bf6a6f88485f82dab04875bd7f2afed865d4e56b246f19dea76728a4946ec195d2fc82db25d1b169be625335cd49c312dadc2188887591d055eacd2218816d6e0eb568ce5a6a4d8ae8484d8b9a4598c681d677512bb1ce3f93529008170ff92911286c157c9403f3270424fd16aba99abf4b7b7b127c2587a0d3202442ab0d5b41aa065abcc771121f7605494f11278273ac1344ddbf3b91f51863ed9a95599ddbf068b65e6eaa077414cedfa110f5d50faabd6731ab7e80f087b9b77b5f8829610242e2afca6f4b2b39971f626320cbdab9699d6ad86fe0089034d691cbe99beb1376c2479cc347a681854ea162b7ce58547a5c259a048a5d9f334d31907b01d629ab2be923bdb63e46e7e6055a042bf3545fd5b3919836e283e0a02");
}


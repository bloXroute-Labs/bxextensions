#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <string>

#include <utils/common/string_helper.h>
#include <utils/crypto/hash_helper.h>
#include <utils/protocols/bitcoin/btc_consts.h>
#include <utils/protocols/bitcoin/btc_block_message.h>
#include <utils/protocols/bitcoin/btc_message_helper.h>
#include <utils/common/buffer_view.h>

typedef utils::common::BufferView BufferView_t;
typedef std::shared_ptr<std::vector<uint8_t>> PVec_t;

class BtcBlockMessageTest : public ::testing::Test {
public:
    PVec_t hex_string_to_vec(std::string& hex_string);
};

PVec_t BtcBlockMessageTest::hex_string_to_vec(std::string& hex_string)
{
    std::vector<uint8_t> vec;
    utils::common::from_hex_string(hex_string, vec);
    return std::make_shared<std::vector<uint8_t>>(vec);
}


TEST_F(BtcBlockMessageTest, test_block1)
{
    // set the desired transactions
    PVec_t vec(nullptr);

    std::array<std::string, 11> desired_txs_hex_string_vector {
        "040a9221f6b5545e3c40e3749b7ff627adc97d4f45efd3517cc49d45d9b58fd0",
        "17e9661eb784510a8ae1e2fba3750d73cbfd7157c706f380d01c2b15f4c43251",
        "24f4bd57635a2e3c85b18f0edf9c1f9bb477cc75c0659f2d065824d9df2c04be",
        "64656ef739413a4b504c92a37912cdec9918a8de8a416c3fe8ced8f576188bb0",
        "85a0b317a161e5568ba90daed07defb2ad75da1393e316ad586e7a467eb9c3cd",
        "b206a37a4901469d18e12fa7910a1cf9347131c906c0ab211ba313428cbe2ba8",
        "c6dbc6ed4473dd9b4e0867c798bb5b1490d1c9b0213906bfbd008eed1c52caa0",
        "e6f1071740a5d37163d4d0d5f5e9d6f391d38e0572094c6621b8c16819db3d68",
        "e730b56de2c1ec57122c913565ab1bd3c4b29ffaf23082fcc307855d26048695",
        "ed2e3b1ffe1aa6d68429d191b67eca2f520011c656e8e5e7658c4e3f9cfeab2b",
        "f2466bb46fbdbf86f72aa9434f78ba7af3aac8d5aa80c47dd5f5a087aed48ad8",
    };

    // read the compact_short_id string, convert it to short id and add to the map
//    for (auto compact_short_id_hex_string : desired_txs_hex_string_vector)
//    {
//        vec = std::move(hex_string_to_vec(compact_short_id_hex_string));
//        BufferView_t compact_sid_buffer_view(*vec);
//        utils::crypto::CompactShortId compact_short_id(compact_sid_buffer_view, 0);
//        desired_compact_short_ids.push_back(compact_short_id.compact_short_id());
//    }

    // Parse block
    std::string block_message_hex_string = "dab5bffa626c6f636b00000000000000e40c00008a8332a90000002011bf3e2bd32bfa7393f12481053311721563a44b2d70f82d016892d8ddc1bf68abce352f93a8863c211964ba40440b98d85bd45765904c31f08f2489afcb9eadca67dc5cffff7f20010000000b02000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0d01660101082f454233322e302fffffffff012afe052a01000000232102a0f2d61a2b8cf7dc750d36914e4834ec6f5bb5a7f9979305ae4a687c3a152626ac000000000200000002b08b1876f5d8cee83f6c418adea81899eccd1279a3924c504b3a4139f76e6564010000006b483045022100f46036bd6f2dce62af9fd849aee12b934956a1992f10ef40dfbb02247fac92c902206bdacfc5ef2737db5c491c32cffd1fa3afb2a823fd5b0d2b5c7fab6afeb2aead412103ca689547cff4c5d764906d26499c1454395a513f20dea9c099f660b3dd3f0402feffffff2babfe9c3f4e8c65e7e5e856c61100522fca7eb691d12984d6a61afe1f3b2eed010000006b4830450221008b9b550701f512c5e7d3d0e0d1665ea93548b3f412d4482115555184601cdbd70220196da0d2d32756e4b062ea7023d1a0ebbe9412708a8ac6c06f45999338a18c6d412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffff0280969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188aca8909800000000001976a914ed40d7d846a8d6fcc33276dbf81c477d110b23c688ac6500000002000000012babfe9c3f4e8c65e7e5e856c61100522fca7eb691d12984d6a61afe1f3b2eed000000006b483045022100f84fa0ff6a87b3a031dfccc98521f841b210456c96bf25221872037c5281c27a0220191ee197092177637f1e224a8239f5f504e1d2f72d8041430b904e9021dcc620412103ad859644a3f56390f28d25b20fdfb37209705b1d08d62966ab5676baffbdf27afeffffff0280969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac9a94a327010000001976a914c4b1f46dbad7dfbef87a6850da4db77f613647d488ac650000000200000002a82bbe8c4213a31b21abc006c9317134f91c0a91a72fe1189d4601497aa306b2000000006b483045022100aeea1993a6da728d90361194540b53769e0a96a1bbf1192410c9239f3b30505802207b44c4be08dfc4e6d6c75e5578c22a698ef8fee595ce1c51d3e017235cd50a5d412102543c8dda542a4fa17092b7c63ccadae0aad0dbf92e88eb55b9fe37d9d75e85cafeffffffa82bbe8c4213a31b21abc006c9317134f91c0a91a72fe1189d4601497aa306b2010000006a47304402204b1fe7a9de4ab7fe0be0fe8568f0c7e8eefc0f6dce948d91e7bddd4e41977e0102202c6961ff0b8e8e57e07546f472ffed8ddc36fac939e0f771b0d3ea896151b9c2412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffff0280969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac1e929800000000001976a914dc1d1a993b4aa3bc7406d6da463a0b2f71c152df88ac5a00000002000000025132c4f4152b1cd080f306c75771fdcb730d75a3fbe2e18a0a5184b71e66e917000000006b483045022100e004f686ffd4352a40dfeaef1498f9f3a595a277d4f644bd74588970a5a47427022002692b57834fb2421882c986b12038c930adfadd3d778ab17005e28898935f4f412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffffd88ad4ae87a0f5d57dc480aad5c8aaf37aba784f43a92af786bfbd6fb46b46f2010000006b483045022100c403571ca4cee795afbe185e77b6cfc1e4e1a90d4f8a065c137e1f8f4c3de46a0220340fc089fb372f5c3317576c409d992cc00921a5cd2e9d062151482cf415ab2b412103d04329b7dc3d8e5651c1fb739e37e4d3773956b4004440c5bed864709532995dfeffffff02bc8d9800000000001976a9149544931a7370dd88d4087e60a3eac989c2b228f488ac80969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac650000000200000002683ddb1968c1b821664c0972058ed391f3d6e9f5d5d0d46371d3a5401707f1e6000000006b483045022100b75241634770e02de2cbcc9d87be3e95fc9496d6381ccbbd423a2aa571d8946a0220361ba4f3504a331bb96858927505f98d163856d913913998d128b72926ff82db412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffff683ddb1968c1b821664c0972058ed391f3d6e9f5d5d0d46371d3a5401707f1e6010000006b483045022100e1eebd30ff210c548070b6aebe23588c9a5c711969c34cc3355ab038654a948f022005cd1272c3b93056e30e5a5c67664dc77aa0defe9c430f150e072ac539f14aca412102be1bd1a77c58b1214af15acfd29c4e015a70ed9adab8ce9f21a532b4edc47256feffffff0294939800000000001976a914fd46648414af50287f237690eae844b0ae7a9e5888ac80969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac510000000200000001958604265d8507c3fc8230f2fa9fb2c4d31bab6535912c1257ecc1e26db530e7010000006a473044022030fbc8ff3bc1f102d8a2964d96fbcf1c5cc3d3f725488efdecd6c8d40b97626802207885b7d5989ba2eaab2a7e65cd83e823ac3a343341832a00cbd8b760e37db018412103f60cc4ecab76e110469253e628bf66c4fc517c0a97682357f38244bd7b942145feffffff025ec3d428010000001976a9144ffb61739f9781b05b56666d72c1a28997c880db88ac80969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac650000000200000002a0ca521ced8e00bdbf063921b0c9d190145bbb98c767084e9bdd7344edc6dbc6010000006a4730440220634d204299971dbe1974676579c169a7a9fadc4bf8ad0a70345890524b61d53b02201e39c48784d95444ad07190644acfb6182521d9a15344b09d5e054a978a837bb412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffff958604265d8507c3fc8230f2fa9fb2c4d31bab6535912c1257ecc1e26db530e7000000006b483045022100c730f6870c8cc9a4169fb95f39084fd5b03b8802e3beca3e1682e4cf88ee27c102201c5be02d4009419e8d326c340259b73fc8820179f093ff9aabdc81b184bf7d3a412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffff0280969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac0a959800000000001976a914cdd4a4d97ded815b33daf7c04c29f6876ba5447988ac650000000200000001ccc1b706ad44c0463a72623a6008f8b72fae2931643a3125a1521506042f96af0000000049483045022100cbea44677352a1c4c58ae57ac8f47cc2296892f0b63a25a862239f788a1e9968022033835258c79ad8ec118f3ecddc755b101f074057ae93e51f9ecd4d1cb8a44c3541feffffff0280969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188acc05a6d29010000001976a914b30b2534ef74db0aa962bbb799ae3954b04e642088ac650000000200000001a0ca521ced8e00bdbf063921b0c9d190145bbb98c767084e9bdd7344edc6dbc6000000006b483045022100c2c2ce251e70b18b5cb43dd3ecc825ffeeaaee4a54094d69ffc46eaceed0fee802204b644caea2cde782a4d382fc1e505205075ca1f2c8547d31ac2d3f1bc812c8ea41210286352d399cebed78ab865cdda48f9100d560a2b40cc192d8303a70e53cafcc0ffeffffff02fc2b3c28010000001976a91467e598da3dfdc7db230a47fef58f1d4d71bac26388ac80969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac6500000002000000025132c4f4152b1cd080f306c75771fdcb730d75a3fbe2e18a0a5184b71e66e917010000006b48304502210082783e18a441783a01d73b1ff4903c289dfe60f87faebc9022202d820fc55c9802203483e0d0a36d57f5909da1dcfa32578829c74fc57a0e3b02d52087ed9cfe2bbb41210249c38cc355cc1b0f839e805e8616cd0ad33e00f7564ea301fe5def73f58bd7b9feffffffbe042cdfd92458062d9f65c075cc77b49b1f9cdf0e8fb1853c2e5a6357bdf424000000006b483045022100a0b506171a668487cad6fd84a05e741426dbfedf41d3484fbac92956c9f8225b022020bac4daac9a107f03880acbdc7bb9e2fdd976a7c6352924cd534383a7d95549412102551bf1afa64550774331b8c157d5c4cedc424f2d74d016f05c2b86d6493f9157feffffff0280969800000000001976a9144c42b323d77e7ae2f3d88294a5557c78eee292a188ac328f9800000000001976a91496f8161d8758b78dd07a8683141b80fb5155a30b88ac65000000";
    vec = hex_string_to_vec(block_message_hex_string);
    BufferView_t block_message(*vec);
    utils::protocols::bitcoin::BtcBlockMessage btc_block_message(block_message);

    utils::crypto::Sha256 block_hash = btc_block_message.block_hash();
    utils::crypto::Sha256 prev_block_hash = btc_block_message.prev_block_hash();
    uint64_t txs_count;
    size_t offset = btc_block_message.get_tx_count(txs_count);
    size_t off = offset;
    ASSERT_EQ(block_hash.hex_string(), "20a2d5dc06afc3510972e88419b04d720505df5fc3d1f9adbd0ba1c64aef7bdb");
    ASSERT_EQ(prev_block_hash.hex_string(), "68bfc1ddd89268012df8702d4ba46315721133058124f19373fa2bd32b3ebf11");
    ASSERT_EQ(txs_count, desired_txs_hex_string_vector.size()); // we expect 11 txs

    auto desired_sha_iter = desired_txs_hex_string_vector.begin();

    for (size_t i = 0; i < txs_count; ++i)
    {
        size_t witness_offset;
        offset = btc_block_message.get_next_tx_offset(off, witness_offset);
        utils::crypto::Sha256 sha = utils::protocols::bitcoin::get_tx_id(
            block_message,
            off,
            witness_offset,
            offset
        );
        ASSERT_EQ(*desired_sha_iter, sha.hex_string());
        ++desired_sha_iter;
        off = offset;
    }
}

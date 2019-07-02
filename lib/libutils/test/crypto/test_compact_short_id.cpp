#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <utility>
#include <algorithm>

#include "utils/crypto/hash_helper.h"
#include "utils/common/string_helper.h"


class CompactShortIdTest: public ::testing::Test
{
};


TEST_F(CompactShortIdTest, test_sip_logic)
{
//    k0 (706050403020100) k1 (f0e0d0c0b0a0908) [0-7] (706050403020100) [8-15](f0e0d0c0b0a0908) [16-23] (1716151413121110) [24-32] (1f1e1d1c1b1a1918)
//    utils::crypto::SipKey_t sip_key(0x0706050403020100, 0x0f0e0d0c0b0a0908);
	utils::crypto::SipKey_t sip_key(0x586db35a20c6dc65, 0xf4f5ead31fc508d4);

    // compact id and tx hash
    typedef std::pair<std::string, std::string> CompactShortIdAndTxHash_t;

    CompactShortIdAndTxHash_t tx1 = std::make_pair("040a9221f6b5545e3c40e3749b7ff627adc97d4f45efd3517cc49d45d9b58fd0", "15fc7b09311c");
    std::vector<CompactShortIdAndTxHash_t> txs;
    txs.push_back(tx1);

    for (auto tx : txs) {
        std::vector <uint8_t> tx_hash_vec;
        utils::common::from_hex_string(tx.first, tx_hash_vec);
        utils::common::BufferView tx_hash_buffer_view(tx_hash_vec);
        utils::crypto::Sha256 tx_hash(tx_hash_buffer_view);

        std::vector <uint8_t> compact_sid_vec;
        utils::common::from_hex_string(tx.second, compact_sid_vec);
        utils::common::BufferView compact_sid_buffer_view(compact_sid_vec);
        utils::crypto::CompactShortId desired_tx_compact_short_id(compact_sid_buffer_view, 0);

        utils::crypto::CompactShortId tx_compact_short_id(sip_key, tx_hash);

        ASSERT_EQ(tx_compact_short_id.compact_short_id(), desired_tx_compact_short_id.compact_short_id());
    }
}

TEST_F(CompactShortIdTest, test_calculate_compact_short_id)
{
    utils::crypto::SipKey_t sip_key(0x586db35a20c6dc65, 0xf4f5ead31fc508d4);

    // compact id and tx hash
    typedef std::pair<std::string, std::string> CompactShortIdAndTxHash_t;

    CompactShortIdAndTxHash_t tx1  = std::make_pair("040a9221f6b5545e3c40e3749b7ff627adc97d4f45efd3517cc49d45d9b58fd0", "15fc7b09311c");
    CompactShortIdAndTxHash_t tx2  = std::make_pair("17e9661eb784510a8ae1e2fba3750d73cbfd7157c706f380d01c2b15f4c43251", "a51da22d1277");
    CompactShortIdAndTxHash_t tx3  = std::make_pair("24f4bd57635a2e3c85b18f0edf9c1f9bb477cc75c0659f2d065824d9df2c04be", "f6029f689d84");
    CompactShortIdAndTxHash_t tx4  = std::make_pair("64656ef739413a4b504c92a37912cdec9918a8de8a416c3fe8ced8f576188bb0", "f33b55f255f7");
    CompactShortIdAndTxHash_t tx5  = std::make_pair("85a0b317a161e5568ba90daed07defb2ad75da1393e316ad586e7a467eb9c3cd", "4a02dca40e69");
    CompactShortIdAndTxHash_t tx6  = std::make_pair("b206a37a4901469d18e12fa7910a1cf9347131c906c0ab211ba313428cbe2ba8", "616cfcb4c236");
    CompactShortIdAndTxHash_t tx7  = std::make_pair("c6dbc6ed4473dd9b4e0867c798bb5b1490d1c9b0213906bfbd008eed1c52caa0", "3ea0f7ce1115");
    CompactShortIdAndTxHash_t tx8  = std::make_pair("e6f1071740a5d37163d4d0d5f5e9d6f391d38e0572094c6621b8c16819db3d68", "b1f26bb52f3a");
    CompactShortIdAndTxHash_t tx9  = std::make_pair("e730b56de2c1ec57122c913565ab1bd3c4b29ffaf23082fcc307855d26048695", "d616eeb21f07");
    CompactShortIdAndTxHash_t tx10 = std::make_pair("ed2e3b1ffe1aa6d68429d191b67eca2f520011c656e8e5e7658c4e3f9cfeab2b", "12f57c414c07");
    CompactShortIdAndTxHash_t tx11 = std::make_pair("f2466bb46fbdbf86f72aa9434f78ba7af3aac8d5aa80c47dd5f5a087aed48ad8", "693fb416451c");

    std::vector<CompactShortIdAndTxHash_t> txs;
    txs.push_back(tx1);
    txs.push_back(tx2);
    txs.push_back(tx3);
    txs.push_back(tx4);
    txs.push_back(tx5);
    txs.push_back(tx6);
    txs.push_back(tx7);
    txs.push_back(tx8);
    txs.push_back(tx9);
    txs.push_back(tx10);
    txs.push_back(tx11);

    for (auto tx : txs) {
        std::vector <uint8_t> tx_hash_vec;
        utils::common::from_hex_string(tx.first, tx_hash_vec);
        utils::common::BufferView tx_hash_buffer_view(tx_hash_vec);
        utils::crypto::Sha256 tx_hash(tx_hash_buffer_view);

        std::vector <uint8_t> compact_sid_vec;
        utils::common::from_hex_string(tx.second, compact_sid_vec);
        utils::common::BufferView compact_sid_buffer_view(compact_sid_vec);
        utils::crypto::CompactShortId desired_tx_compact_short_id(compact_sid_buffer_view, 0);

        utils::crypto::CompactShortId tx_compact_short_id(sip_key, tx_hash);

        ASSERT_EQ(tx_compact_short_id.compact_short_id(), desired_tx_compact_short_id.compact_short_id());
    }
}


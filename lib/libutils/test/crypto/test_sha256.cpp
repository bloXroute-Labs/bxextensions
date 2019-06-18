#include <gtest/gtest.h>
#include <array>

#include "utils/common/string_helper.h"
#include "utils/crypto/sha256.h"


class Sha256Test: public ::testing::Test
{
};


TEST_F(Sha256Test, test_sha) {
    std::string tx_hash_hex_string = "040a9221f6b5545e3c40e3749b7ff627adc97d4f45efd3517cc49d45d9b58fd0";
    std::vector <uint8_t> tx_hash_vec;
    utils::common::from_hex_string(tx_hash_hex_string, tx_hash_vec);
    utils::common::BufferView tx_hash_buffer_view(tx_hash_vec);

    utils::crypto::Sha256 sha_hex_string(tx_hash_hex_string);
    utils::crypto::Sha256 sha_uint8(tx_hash_vec);
    utils::crypto::Sha256 sha_bf(tx_hash_buffer_view);

    EXPECT_EQ(sha_hex_string, sha_bf);
    EXPECT_EQ(sha_uint8, sha_bf);
    EXPECT_EQ(sha_bf.hex_string(), tx_hash_hex_string);
}
#include <gtest/gtest.h>
#include <array>

#include "utils/common/string_helper.h"
#include "utils/crypto/keccak.h"

typedef utils::crypto::Sha256 Sha256_t;

class Sha3Test: public ::testing::Test
{
};


TEST_F(Sha3Test, test_sha) {
    std::string data_str("sha3 test string");
    std::string sha3_hash_str("4c8beb9a7540ac44b461b2d0deb4b08e09233b062fac874f0c24422a0006caab");
    Sha256_t sha(std::move(utils::crypto::keccak_sha3((uint8_t*) data_str.c_str(), 0,  data_str.size())));
    EXPECT_EQ(sha3_hash_str, sha.hex_string());
}
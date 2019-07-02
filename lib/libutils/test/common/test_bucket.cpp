#include <gtest/gtest.h>
#include <array>

#include "utils/common/bucket.h"


class BucketTest: public ::testing::Test
{
};


TEST_F(BucketTest, test_bucket) {
    utils::common::Bucket<uint32_t> bucket(10, 0);

    for (size_t i = 0; i < 9; ++i) {
        bucket.insert(i);
        EXPECT_FALSE(bucket.is_full());
    }

    for (size_t i = 9; i < 12; ++i) {
        bucket.insert(i);
        EXPECT_TRUE(bucket.is_full());
    }

    for (size_t i = 9; i < 12; ++i) {
        EXPECT_TRUE(bucket.is_full());
        bucket.erase(i);
    }
    EXPECT_FALSE(bucket.is_full());

    for (size_t i = 0; i < 9 ; ++i) {
        EXPECT_TRUE(bucket.contains(i));
    }

    for (size_t i = 9; i < 12 ; ++i) {
        EXPECT_FALSE(bucket.contains(i));
    }
}
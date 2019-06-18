#include <gtest/gtest.h>

#include <utils/concurrency/atomic_operation_helper.h>


/*
 * check compare and swap
 */
TEST(AtomicOperationHelperTest, test_compare_and_swap) {
	uint32_t src = 5, false_compare = 6, true_compare = 5, desired = 8;
	EXPECT_FALSE(
			utils::concurrency::compare_and_swap(
					src, false_compare, desired
			)
	);
	EXPECT_NE(src, desired);
	EXPECT_TRUE(
			utils::concurrency::compare_and_swap(
					src, true_compare, desired
			)
	);

	EXPECT_EQ(src, desired);
}



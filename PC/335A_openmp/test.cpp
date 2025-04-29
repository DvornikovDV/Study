#include "pch.h"
// Тесты для функции parallel_sum
TEST(ParallelSumTest, TestSmallN) {
    EXPECT_EQ(parallel_sum(1), 1);
    EXPECT_EQ(parallel_sum(2), 25);
    EXPECT_EQ(parallel_sum(3), 181465);
    EXPECT_EQ(parallel_sum(7), 7272256138021242073);
}

TEST(ParallelSumTest, TestZeroAndNegativeN) {
    EXPECT_EQ(parallel_sum(0LL), 0);
    EXPECT_EQ(parallel_sum(-1LL), 0);
}
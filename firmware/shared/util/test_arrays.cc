#include "arrays.hpp"
#include "gtest/gtest.h"

TEST(SharedUtil, Arrays) {
    using namespace shared::util;

    float data[6] = {1, 4, -10, 4, 7, 6};

    int idx;
    float max = GetMaximum<float, 6>(data, &idx);
    EXPECT_EQ(max, 7.f);
    EXPECT_EQ(idx, 4);

    float min = GetMinimum<float, 6>(data, &idx);
    EXPECT_EQ(min, -10.f);
    EXPECT_EQ(idx, 2);

    float avg = GetAverage<float, 6>(data);
    EXPECT_FLOAT_EQ(avg, 2);
}

TEST(SharedUtil, ArraysDuplicate) {
    using namespace shared::util;

    float data[5] = {2, -4, -4, 3, 3};

    int idx;

    float min = GetMinimum<float, 5>(data, &idx);
    ASSERT_EQ(min, -4.f);
    EXPECT_EQ(idx, 1) << "Should return the index of the first minimum.";

    float max = GetMaximum<float, 5>(data, &idx);
    ASSERT_EQ(max, 3.f);
    EXPECT_EQ(idx, 3) << "Should return the index of the first maximum.";
}
#include "etl/circular_buffer.h"
#include "gtest/gtest.h"
#include "moving_average.hpp"

using macfe::MovingAverage;

TEST(SharedUtil, CircularBuffer) {
    // Make sure this works as expected
    etl::circular_buffer<int, 3> b;

    b.push(1);
    b.push(2);
    ASSERT_EQ(b.front(), 1);
    b.push(3);
    ASSERT_EQ(b.front(), 1) << "Buffer should not start overwriting yet.";
    b.push(4);
    ASSERT_EQ(b.front(), 2);
}

TEST(SharedUtil, MovingAverage) {
    const size_t LENGTH = 10;
    const float LENGTHf = static_cast<float>(LENGTH);

    MovingAverage<LENGTH> ma;

    EXPECT_FLOAT_EQ(ma.GetValue(), 0);

    for (int i = 1; i <= LENGTH; i++) {
        ma.LoadValue(1.f);
        EXPECT_FLOAT_EQ(ma.GetValue(), i / LENGTHf);
    }

    ma.LoadValue(1.f);
    EXPECT_FLOAT_EQ(ma.GetValue(), 1.f)
        << "MovingAverage should be saturated now.";

    ma.LoadValue(LENGTH + 1);
    EXPECT_FLOAT_EQ(ma.GetValue(), 2.f);
}

TEST(SharedUtil, MovingAverage1) {
    // Moving average with length 1 should just return the most recent value
    // You wouldn't ever use such an object. It it just here to test the edge
    // case.

    MovingAverage<1> ma;

    for (int i = 0; i <= 100; i++) {
        ma.LoadValue(i);
        EXPECT_FLOAT_EQ(ma.GetValue(), static_cast<float>(i));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
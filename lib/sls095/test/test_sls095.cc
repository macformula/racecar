#include <gtest/gtest.h>

#include "sls095.hpp"

TEST(VoltageToPosition, ZeroVoltage) {
    EXPECT_FLOAT_EQ(0, macfe::sls095::VoltToMillimeter(0, 5.0f));
}

TEST(VoltageToPosition, MaxVoltage) {
    EXPECT_FLOAT_EQ(75, macfe::sls095::VoltToMillimeter(
                            5.0f, 5.0f));  // maximum position of 75mm
}

TEST(VoltageToPosition, MiddleVoltage) {
    EXPECT_FLOAT_EQ(37.5f, macfe::sls095::VoltToMillimeter(2.5f, 5.0f));
}

TEST(VoltageToPosition, NegativeSupplyReturnsZero) {
    EXPECT_FLOAT_EQ(0.0f, macfe::sls095::VoltToMillimeter(2.0f, -5.0f));
}

TEST(VoltageToPosition, MidWith33vSupply) {
    // 75 * 1.65 / 3.3 = 37.5
    EXPECT_FLOAT_EQ(37.5f, macfe::sls095::VoltToMillimeter(1.65f, 3.3f));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

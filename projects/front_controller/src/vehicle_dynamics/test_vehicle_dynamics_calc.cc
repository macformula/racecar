#include <gtest/gtest.h>

#include "vehicle_dynamics_calc.hpp"

using namespace ctrl;

TEST(VehicleDynamicsCalc, CreateTorqueVectoringFactor) {
    EXPECT_FLOAT_EQ(CreateTorqueVectoringFactor(5.0), 0.934);
    EXPECT_FLOAT_EQ(CreateTorqueVectoringFactor(10.0), 0.87);
    EXPECT_FLOAT_EQ(CreateTorqueVectoringFactor(13.2), 0.83032);
    EXPECT_FLOAT_EQ(CreateTorqueVectoringFactor(23.3), 0.70476);
    EXPECT_FLOAT_EQ(CreateTorqueVectoringFactor(27.3), 0.683);
}

TEST(VehicleDynamicsCalc, AdjustTorqueVectoring) {
    {
        TorqueVector tv = AdjustTorqueVectoring(15.0);
        EXPECT_FLOAT_EQ(tv.left, 1.0);
        EXPECT_FLOAT_EQ(tv.right, 0.808);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(-10.0);
        EXPECT_FLOAT_EQ(tv.left, 0.87);
        EXPECT_FLOAT_EQ(tv.right, 1);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(0.0);
        EXPECT_FLOAT_EQ(tv.left, 1);
        EXPECT_FLOAT_EQ(tv.right, 1);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(13.8);
        EXPECT_FLOAT_EQ(tv.left, 1);
        EXPECT_FLOAT_EQ(tv.right, 0.82288);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(-6.7);
        EXPECT_FLOAT_EQ(tv.left, 0.91224);
        EXPECT_FLOAT_EQ(tv.right, 1);
    }
}

TEST(VehicleDynamicsCalc, TestMultistageTC) {
    int time_ms = 0;
    TractionControl tc;
    tc.Init(time_ms);
    // Continue holding at 1
    for (; time_ms <= 200; time_ms += 1) {
        EXPECT_FLOAT_EQ(tc.UpdateScaleFactor(0.05, 0.1, time_ms), 1);
    }
    // Continue holding at 1 for 49 msec even after we start slipping
    for (; time_ms <= 250; time_ms += 1) {
        EXPECT_FLOAT_EQ(tc.UpdateScaleFactor(0.15, 0.1, time_ms), 1);
    }
    // Then drop to 0 and stay there while we slip
    for (; time_ms <= 300; time_ms += 1) {
        EXPECT_FLOAT_EQ(tc.UpdateScaleFactor(0.15, 0.1, time_ms), 0);
    }
    // Stop slipping, start ramping again
    for (; time_ms <= 400; time_ms += 1) {
        EXPECT_FLOAT_EQ(tc.UpdateScaleFactor(0.05, 0.1, time_ms),
                        (float)(time_ms - 300) / 100.);
    }
}

TEST(VehicleDynamicsCalc, TestActualSlip) {
    // Should return 0 because right-rear wheel speed is greater than idle wheel
    // speed, forcing the bound to 0.
    EXPECT_FLOAT_EQ(ctrl::CalculateActualSlip(132.5, 134.0, 140.0, 135.0), 0);

    // Same as the last test but Left Rear wheel speed is used because it is
    // greater then Right-Rear wheel speed.
    EXPECT_FLOAT_EQ(ctrl::CalculateActualSlip(133.4, 130.2, 140.0, 135.0), 0);

    // Should be a decimal since Right-Rear wheel speed is less than the idle
    // speed, and the Actual-Slip value will return.
    EXPECT_NEAR(ctrl::CalculateActualSlip(155.6, 157.2, 155.3, 157.8), 0.004152,
                1e-6);

    // Should be a decimal for same reasons as previous, but for Left-Rear wheel
    // speed.
    EXPECT_NEAR(ctrl::CalculateActualSlip(156.4, 155.3, 155.2, 157.1), 0.001601,
                1e-6);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

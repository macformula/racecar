#include <gtest/gtest.h>

#include "sensors/dynamics/dynamics.hpp"
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
    // Should return 0 because rear wheel speed is less than idle front wheel
    // speed, forcing the bound to 0 (deceleration/braking).
    // idle = (140.0 + 135.0) / 2 = 137.5. max_rear = 134.0.
    EXPECT_FLOAT_EQ(ctrl::CalculateActualSlip({
                        .front_left = 140.0f,
                        .front_right = 135.0f,
                        .rear_left = 132.5f,
                        .rear_right = 134.0f,
                    }),
                    0.0f);

    // Same as above but Left-Rear wheel speed is used because it is greater
    // than Right-Rear wheel speed, but still less than idle speed.
    // idle = 137.5. max_rear = 133.4.
    EXPECT_FLOAT_EQ(ctrl::CalculateActualSlip({
                        .front_left = 140.0f,
                        .front_right = 135.0f,
                        .rear_left = 133.4f,
                        .rear_right = 130.2f,
                    }),
                    0.0f);

    // Right-Rear wheel speed is greater than idle front speed.
    // idle = (155.3 + 157.8) / 2 = 156.55. max_rear = 157.2.
    // slip = (157.2 / 156.55) - 1.0 = 0.004152.
    EXPECT_NEAR(ctrl::CalculateActualSlip({
                    .front_left = 155.3f,
                    .front_right = 157.8f,
                    .rear_left = 155.6f,
                    .rear_right = 157.2f,
                }),
                0.004152f, 1e-6f);

    // Same as above, but Left-Rear wheel speed is used because it is greater
    // than Right-Rear wheel speed.
    // idle = (155.2 + 157.1) / 2 = 156.15. max_rear = 156.4.
    // slip = (156.4 / 156.15) - 1.0 = 0.001601.
    EXPECT_NEAR(ctrl::CalculateActualSlip({
                    .front_left = 155.2f,
                    .front_right = 157.1f,
                    .rear_left = 156.4f,
                    .rear_right = 155.3f,
                }),
                0.001601f, 1e-6f);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

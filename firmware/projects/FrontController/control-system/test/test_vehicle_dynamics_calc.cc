#include <iostream>

#include "control-system/vehicle_dynamics_calc.hpp"
#include "testing.h"

using namespace ctrl;

void TestTorqueRequest() {
    TorqueRequest tr;
    std::cout << "Testing TestTorqueRequest" << std::endl;
    // Test cases for ComputeTorqueRequest
    ASSERT_CLOSE(tr.Update(30.0, 20.0),
                 0.0);  // Should return 0.0 due to State::Stop
    ASSERT_CLOSE(tr.Update(15.0, 2.5),
                 0.0);  // Only the !brake_on condition is met here, so it
                        // should stay in State::Stop and return 0.0
    ASSERT_CLOSE(
        tr.Update(4.5, 10.5),
        0.0);  // Only the driver_torque_request < static_cast<T>(5) condition
               // is met here, so it should stay in State::Stop and return 0.0
    ASSERT_CLOSE(
        tr.Update(3.0, 0.0),
        3.0);  // Should move to State::Run and return driver_torque_request
    ASSERT_CLOSE(tr.Update(40.0, 4.0),
                 40.0);  // Should stay in State::Run and return 40.0
    ASSERT_CLOSE(tr.Update(12.2, 11.0),
                 12.2);  // Only the brake_on condition is met here, so it
                         // should stay in State::Run and return 12.2
    ASSERT_CLOSE(tr.Update(25.4, 9.0),
                 25.4);  // Only the driver_torque_request >= static_cast<T>(25)
                         // condition is met here, so it should stay in
                         // State::Run and return 25.4
}

void TestTorqueVectoring() {
    std::cout << "Testing TestTorqueVectoring" << std::endl;
    ASSERT_CLOSE(CreateTorqueVectoringFactor(5.0), 0.934);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(10.0), 0.87);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(13.2), 0.83032);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(23.3), 0.70476);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(27.3), 0.683);

    {
        TorqueVector tv = AdjustTorqueVectoring(15.0);
        ASSERT_CLOSE(tv.left, 1.0);
        ASSERT_CLOSE(tv.right, 0.808);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(-10.0);
        ASSERT_CLOSE(tv.left, 0.87);
        ASSERT_CLOSE(tv.right, 1);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(0.0);
        ASSERT_CLOSE(tv.left, 1);
        ASSERT_CLOSE(tv.right, 1);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(13.8);
        ASSERT_CLOSE(tv.left, 1);
        ASSERT_CLOSE(tv.right, 0.82288);
    }

    {
        TorqueVector tv = AdjustTorqueVectoring(-6.7);
        ASSERT_CLOSE(tv.left, 0.91224);
        ASSERT_CLOSE(tv.right, 1);
    }
}

void TestMultistageTC() {
    std::cout << "Testing TestMultistageTC" << std::endl;

    int time_ms = 0;
    TractionControl tc;
    tc.Init(time_ms);

    // Continue holding at 1
    for (; time_ms <= 200; time_ms += 1) {
        ASSERT_CLOSE(tc.UpdateScaleFactor(0.05, 0.1, time_ms), 1);
    }
    // Continue holding at 1 for 49 msec even after we start slipping
    for (; time_ms <= 250; time_ms += 1) {
        ASSERT_CLOSE(tc.UpdateScaleFactor(0.15, 0.1, time_ms), 1);
    }
    // Then drop to 0 and stay there while we slip
    for (; time_ms <= 300; time_ms += 1) {
        ASSERT_CLOSE(tc.UpdateScaleFactor(0.15, 0.1, time_ms), 0);
    }
    // Stop slipping, start ramping again
    for (; time_ms <= 400; time_ms += 1) {
        ASSERT_CLOSE(tc.UpdateScaleFactor(0.05, 0.1, time_ms),
                     (float)(time_ms - 300) / 100.);
    }
}

void TestActualSlip() {
    std::cout << "Testing TestActualSlip" << std::endl;
    // Should return 0 because right-rear wheel speed is greater than idle wheel
    // speed, forcing the bound to 0.
    ASSERT_CLOSE(ctrl::CalculateActualSlip(132.5, 134.0, 140.0, 135.0), 0);

    // Same as the last test but Left Rear wheel speed is used because it is
    // greater then Right-Rear wheel speed.
    ASSERT_CLOSE(ctrl::CalculateActualSlip(133.4, 130.2, 140.0, 135.0), 0);

    // Should be a decimal since Right-Rear wheel speed is less than the idle
    // speed, and the Actual-Slip value will return.
    ASSERT_CLOSE(ctrl::CalculateActualSlip(155.6, 157.2, 155.3, 157.8),
                 0.004152);

    // Should be a decimal for same reasons as previous, but for Left-Rear wheel
    // speed.
    ASSERT_CLOSE(ctrl::CalculateActualSlip(156.4, 155.3, 155.2, 157.1),
                 0.001601);
}

void VdCalcTest() {
    TestTorqueRequest();
    TestTorqueVectoring();
    TestMultistageTC();
    TestActualSlip();
}
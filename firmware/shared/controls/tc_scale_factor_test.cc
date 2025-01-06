#include <iostream>

#include "tc_scale_factor.h"  // include the header with your function implementation
#include "testing.h"

void TestMultistageTC() {
    // Ramp up while NOT slipping
    int time_ms = 0;
    for (; time_ms <= 100; time_ms += 1) {
        ASSERT_CLOSE(ctrl::CalculateTCScaleFactor(0.05, 0.1, time_ms),
                     (double)time_ms / 100.);
    }
    // Continue holding at 1
    for (; time_ms <= 200; time_ms += 1) {
        ASSERT_CLOSE(ctrl::CalculateTCScaleFactor(0.05, 0.1, time_ms), 1);
    }
    // Continue holding at 1 for 49 msec even after we start slipping
    for (; time_ms <= 250; time_ms += 1) {
        ASSERT_CLOSE(ctrl::CalculateTCScaleFactor(0.15, 0.1, time_ms), 1);
    }
    // Then drop to 0 and stay there while we slip
    for (; time_ms <= 300; time_ms += 1) {
        ASSERT_CLOSE(ctrl::CalculateTCScaleFactor(0.15, 0.1, time_ms), 0);
    }
    // Stop slipping, start ramping again
    for (; time_ms <= 400; time_ms += 1) {
        ASSERT_CLOSE(ctrl::CalculateTCScaleFactor(0.05, 0.1, time_ms),
                     (double)(time_ms - 300) / 100.);
    }
}

int main() {
    // Write your test cases.

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

    TestMultistageTC();

    // This statement will not be reached if an assert fails.
    std::cout << "All tests passed" << std::endl;
}
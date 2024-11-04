#include "tc_scale_factor.h"  // include the header with your function implementation

#include <iostream>

#include "testing.h"

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

    // (Unbounded) Should return 0 because stateflow functionality is not
    // introduced yet, so function returns 0.
    ASSERT_CLOSE(ctrl::TCStateflowChart(0.0042, 0.2), 0);

    // (Bounded by 0) Same as previous test
    ASSERT_CLOSE(ctrl::TCStateflowChart(0.0, 0.2), 0);

    // Should return the output value from the TC Stateflow Chart function
    ASSERT_CLOSE(ctrl::CalculateTCScaleFactor(0.5), 0.5);

    // Should return the output value from the TC Stateflow Chart function
    ASSERT_EQ(ctrl::CalculateTCScaleFactor(3), 3);

    // This statement will not be reached if an assert fails.
    std::cout << "All tests passed" << std::endl;
}
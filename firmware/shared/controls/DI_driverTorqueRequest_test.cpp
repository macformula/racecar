// don't need `#pragma once` in a .cc / source file

#include "DI_driverTorqueRequest.h"  // include the header with your function implementation

#include <iostream>

#include "testing.h"

int main() {
    // Write your test cases.

    ASSERT_CLOSE(ctrl::DI_driverTorqueRequest(false, true, 2.5), 0.0);      // should return 0
    ASSERT_CLOSE(ctrl::DI_driverTorqueRequest(true, false, 2.5), 1.0);      // should return AccelPedPos = 1 (edge case)
    ASSERT_CLOSE(ctrl::DI_driverTorqueRequest(true, false, 0.6f), 1.0f);      // should return round(AccelPedPos) = 1
    ASSERT_NCLOSE(ctrl::DI_driverTorqueRequest(true, false, 0.4), 2.0);      // should return AccelPedPos = 0

    ASSERT_EQ(ctrl::DI_driverTorqueRequest(true, false, -1), 0);            //should return AccelPedPos = 0 (negative position case)

    // This statement will not be reached if an assert fails.
    std::cout << "All tests passed" << std::endl;
}
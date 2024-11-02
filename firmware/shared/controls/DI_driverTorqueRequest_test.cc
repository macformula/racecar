// don't need `#pragma once` in a .cc / source file

#include "DI_driverTorqueRequest.h"  // include the header with your function implementation

#include <iostream>

#include "testing.h"

int main() {
    // Write your test cases.

    // should return 0
    ASSERT_CLOSE(ctrl::DI_driverTorqueRequest(false, true, 2.5), 0);

    // should return AccelPedPos = 2.5
    ASSERT_CLOSE(ctrl::DI_driverTorqueRequest(true, false, 2.5), 2.5);

    // should return AccelPedPos = 0.0006f
    ASSERT_CLOSE(ctrl::DI_driverTorqueRequest(true, false, 0.0006f), 0.0006f);

    // should return AccelPedPos = 0.4
    ASSERT_NCLOSE(ctrl::DI_driverTorqueRequest(true, false, 0.4), 0.5);

    // This statement will not be reached if an assert fails.
    std::cout << "All tests passed" << std::endl;
}
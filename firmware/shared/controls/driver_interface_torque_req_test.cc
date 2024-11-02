// don't need `#pragma once` in a .cc / source file

#include "driver_interface_torque_req.h"  // include the header with your function implementation

#include <iostream>

#include "testing.h"

int main() {
    // Write your test cases.

    // should return 0
    ASSERT_CLOSE(ctrl::driver_interface_torque_req(false, false, 2.5), 0);

    // should return accel_ped_pos = 2.5
    ASSERT_CLOSE(ctrl::driver_interface_torque_req(true, false, 2.5), 2.5);

    // should return accel_ped_pos = 0.0006f
    ASSERT_CLOSE(ctrl::driver_interface_torque_req(true, false, 0.0006f),
                 0.0006f);

    // should return accel_ped_pos = 0.4
    ASSERT_NCLOSE(ctrl::driver_interface_torque_req(false, true, 0.4), 0.5);

    // should return 0
    ASSERT_EQ(ctrl::driver_interface_torque_req(true, true, 1), 0);

    // This statement will not be reached if an assert fails.
    std::cout << "All tests passed" << std::endl;
}
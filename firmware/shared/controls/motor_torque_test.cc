#include "motor_torque.h"

#include <iostream>

#include "testing.h"
#include "tvFactor.h"

int main() {
    using namespace ctrl;

    // Test cases for ComputeTorqueRequest
    ASSERT_CLOSE(ComputeTorqueRequest(30.0, 20.0),
                 0.0);  // Should return 0.0 due to State::Stop
    ASSERT_CLOSE(ComputeTorqueRequest(15.0, 2.5),
                 0.0);  // Only the !brake_on condition is met here, so it
                        // should stay in State::Stop and return 0.0
    ASSERT_CLOSE(
        ComputeTorqueRequest(4.5, 10.5),
        0.0);  // Only the driver_torque_request < static_cast<T>(5) condition
               // is met here, so it should stay in State::Stop and return 0.0
    ASSERT_CLOSE(
        ComputeTorqueRequest(3.0, 0.0),
        3.0);  // Should move to State::Run and return driver_torque_request
    ASSERT_CLOSE(ComputeTorqueRequest(40.0, 4.0),
                 40.0);  // Should stay in State::Run and return 40.0
    ASSERT_CLOSE(ComputeTorqueRequest(12.2, 11.0),
                 12.2);  // Only the brake_on condition is met here, so it
                         // should stay in State::Run and return 12.2
    ASSERT_CLOSE(ComputeTorqueRequest(25.4, 9.0),
                 25.4);  // Only the driver_torque_request >= static_cast<T>(25)
                         // condition is met here, so it should stay in
                         // State::Run and return 25.4
    std::cout << "All test cases passed!" << std::endl;

    return 0;
}
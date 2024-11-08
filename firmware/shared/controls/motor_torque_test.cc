#include "motor_torque.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test cases for CalculateMotorTorque
    // Test Case 1: Smaller Sample Sizes
    {
        double raw_torque_values[] = {0.05, 0.1, 0.15, 0.2};
        auto result = CalculateMotorTorque(raw_torque_values, 4, 1.5, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 187.5);
        ASSERT_CLOSE(std::get<1>(result), 150);
    }

    {
        double raw_torque_values[] = {0.02, 0.04, 0.06};
        auto result = CalculateMotorTorque(raw_torque_values, 3, 1.1, 1.3);
        ASSERT_CLOSE(std::get<0>(result), 44.0);
        ASSERT_CLOSE(std::get<1>(result), 52);
    }

    // Test Case 2: Larger Sample Sizes
    {
        double raw_torque_values[] = {0.8,  0.21, 0.34, 0.43, 0.27,
                                      0.47, 0.46, 0.21, 0.73};
        auto result = CalculateMotorTorque(raw_torque_values, 9, 1.5, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 435.55556 * 1.5);
        ASSERT_CLOSE(std::get<1>(result), 435.55556 * 1.2);
    }

    // Test Case 3: Sample Size over 10 Entities
    {
        double raw_torque_values[] = {0.8,  0.21, 0.34, 0.43, 0.27, 0.47, 0.46,
                                      0.21, 0.73, 0.12, 0.11, 0.11, 0.14, 0.12};
        auto result = CalculateMotorTorque(raw_torque_values, 14, 1.1, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 274 * 1.1);
        ASSERT_CLOSE(std::get<1>(result), 274 * 1.2);
    }

    // Test Case 4: Zero values in raw_torque_values
    {
        double raw_torque_values[] = {0.0, 0.0, 0.0};
        auto result = CalculateMotorTorque(raw_torque_values, 3, 1.8, 1.95);
        ASSERT_CLOSE(std::get<0>(result), 0);
        ASSERT_CLOSE(std::get<1>(result), 0);
    }

    // Test Case 5: Single element in raw_torque_values
    {
        double raw_torque_values[] = {0.1};
        auto result = CalculateMotorTorque(raw_torque_values, 1, 1.5, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 150);
        ASSERT_CLOSE(std::get<1>(result), 120);
    }

    // Test cases for CheckBrakePedalPosition
    ASSERT_FALSE(CheckBrakePedalPosition(0.09));
    ASSERT_FALSE(CheckBrakePedalPosition(0.1));
    ASSERT_TRUE(CheckBrakePedalPosition(0.15));

    // Test cases for ComputeTorqueRequest
    // Have tested for starting in Run State, but cannot test for it without
    // changing the original code (finite state machine) The code CAN be updated
    // in order to pass its past state in as a parameter, but I dont know how we
    // want to tackle finite state machine yet
    ASSERT_CLOSE(ComputeTorqueRequest(0.3, 0.2),
                 0.0);  // Should return 0.0 due to State::Stop
    ASSERT_CLOSE(
        ComputeTorqueRequest(0.03, 0.0),
        0.03);  // Should stay in State::Run and return driver_torque_request
    ASSERT_CLOSE(ComputeTorqueRequest(0.4, 0.04),
                 0.0);  // Should not switch to State::Run and return 0.0

    std::cout << "All test cases passed!" << std::endl;

    return 0;
}
#include "motor_torque.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test cases for CalculateMotorTorque
    // Test Case 1: Smaller Sample Sizes
    {
        double raw_torque_values[] = {5, 10, 15, 20};
        auto result = CalculateMotorTorque(raw_torque_values, 4, 1.5, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 187.5);
        ASSERT_CLOSE(std::get<1>(result), 150);
    }

    {
        double raw_torque_values[] = {2, 4, 6};
        auto result = CalculateMotorTorque(raw_torque_values, 3, 1.1, 1.3);
        ASSERT_CLOSE(std::get<0>(result), 44);
        ASSERT_CLOSE(std::get<1>(result), 52);
    }

    // Test Case 2: Larger Sample Sizes
    {
        double raw_torque_values[] = {80, 21, 34, 43, 27, 47, 46, 21, 73};
        auto result = CalculateMotorTorque(raw_torque_values, 9, 1.5, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 435.55556 * 1.5);
        ASSERT_CLOSE(std::get<1>(result), 435.55556 * 1.2);
    }

    // Test Case 3: Sample Size over 10 Entities
    {
        double raw_torque_values[] = {80, 21, 34, 43, 27, 47, 46,
                                      21, 73, 12, 11, 11, 14, 12};
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
        double raw_torque_values[] = {10};
        auto result = CalculateMotorTorque(raw_torque_values, 1, 1.5, 1.2);
        ASSERT_CLOSE(std::get<0>(result), 150);
        ASSERT_CLOSE(std::get<1>(result), 120);
    }

    // Test cases for CheckBrakePedalPosition
    ASSERT_FALSE(CheckBrakePedalPosition(9));
    ASSERT_FALSE(CheckBrakePedalPosition(10));
    ASSERT_TRUE(CheckBrakePedalPosition(15));

    // Test cases for ComputeTorqueRequest
    // Have tested for starting in Run State, but cannot test for it without
    // changing the original code (finite state machine) The code CAN be updated
    // in order to pass its past state in as a parameter, but I dont know how we
    // want to tackle finite state machine yet
    ASSERT_CLOSE(ComputeTorqueRequest(30.0, 20.0),
                 0.0);  // Should return 0.0 due to State::Stop
    ASSERT_CLOSE(
        ComputeTorqueRequest(3.0, 0.0),
        3.0);  // Should stay in State::Run and return driver_torque_request
    ASSERT_CLOSE(ComputeTorqueRequest(40.0, 4.0),
                 0.0);  // Should not switch to State::Run and return 0.0

    std::cout << "All test cases passed!" << std::endl;

    return 0;
}
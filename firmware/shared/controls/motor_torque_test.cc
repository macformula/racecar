#include "motor_torque.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test cases for CalculateMotorTorque
    // Test Case 1: Smaller Sample Sizes
    {
        double new_torque_values[] = {5.0, 10.0, 15.0};
        for (int i = 0; i < 3; i++) {
            CalculateMotorTorque(new_torque_values[i], 1.5, 1.2, i == 0);
        }
        auto [right_motor_torque, left_motor_torque] =
            CalculateMotorTorque(20.0, 1.5, 1.2);
        ASSERT_CLOSE(right_motor_torque, 187.5);
        ASSERT_CLOSE(left_motor_torque, 150.0);
    }

    {
        double new_torque_values[] = {2.0, 4.0};
        for (int i = 0; i < 2; i++) {
            CalculateMotorTorque(new_torque_values[i], 1.1, 1.3, i == 0);
        }
        auto [right_motor_torque, left_motor_torque] =
            CalculateMotorTorque(6.0, 1.1, 1.3);
        ASSERT_CLOSE(right_motor_torque, 44.0);
        ASSERT_CLOSE(left_motor_torque, 52.0);
    }

    // Test Case 2: Larger Sample Sizes
    {
        for (double i = 1; i <= 9; i++) {
            CalculateMotorTorque(i, 1.5, 1.2, i == 1);
        }
        auto [right_motor_torque, left_motor_torque] =
            CalculateMotorTorque(10.0, 1.5, 1.2);
        ASSERT_CLOSE(right_motor_torque, 82.5);
        ASSERT_CLOSE(left_motor_torque, 66.0);
    }

    // Test Case 3: Sample Size over 10 Entities
    {
        double new_torque_values[] = {80, 21, 34, 43, 27, 47, 46,
                                      21, 73, 12, 11, 11, 14};
        for (int i = 0; i < 13; i++) {
            CalculateMotorTorque(new_torque_values[i], 1.1, 1.3, i == 0);
        }
        auto [right_motor_torque, left_motor_torque] =
            CalculateMotorTorque(12.0, 1.1, 1.2);
        ASSERT_CLOSE(right_motor_torque, 274 * 1.1);
        ASSERT_CLOSE(left_motor_torque, 274 * 1.2);
    }

    // Test Case 4: Zero values in raw_torque_values
    {
        double new_torque_values[] = {0.0, 0.0, 0.0};
        for (int i = 0; i < 3; i++) {
            CalculateMotorTorque(new_torque_values[i], 1.1, 1.2, i == 0);
        }
        auto [right_motor_torque, left_motor_torque] =
            CalculateMotorTorque(0.0, 1.8, 1.95);
        ASSERT_CLOSE(right_motor_torque, 0.0);
        ASSERT_CLOSE(left_motor_torque, 0.0);
    }

    // Test Case 5: Single element in raw_torque_values
    {
        double new_torque_value = 10.0;
        auto [right_motor_torque, left_motor_torque] =
            CalculateMotorTorque(new_torque_value, 1.5, 1.2, true);
        ASSERT_CLOSE(right_motor_torque, 150.0);
        ASSERT_CLOSE(left_motor_torque, 120.0);
    }

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
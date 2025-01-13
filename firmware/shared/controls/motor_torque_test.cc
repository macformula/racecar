#include "motor_torque.h"
#include "tvFactor.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test cases for CalculateMotorTorque
    // Test Case 1: Smaller Sample Sizes
    {
        TorqueVector<double> tv = {
            .left_torque_vector = 1.5,
            .right_torque_vector = 1.2
        };

        double new_torque_values[] = {5.0, 10.0, 15.0};
        for (int i = 0; i < 3; i++) {
            CalculateMotorTorque(new_torque_values[i], tv, i == 0);
        }
        MotorTorque<double> motor_torque =
            CalculateMotorTorque(20.0, tv);
        ASSERT_CLOSE(motor_torque.right_motor_torque_limit, 18.75);
        ASSERT_CLOSE(motor_torque.left_motor_torque_limit, 15.00);
    }

    {
        TorqueVector<double> tv = {
            .left_torque_vector = 1.1,
            .right_torque_vector = 1.3
        };

        double new_torque_values[] = {2.0, 4.0};
        for (int i = 0; i < 2; i++) {
            CalculateMotorTorque(new_torque_values[i], tv, i == 0);
        }
        MotorTorque<double> motor_torque =
            CalculateMotorTorque(6.0, tv);
        ASSERT_CLOSE(motor_torque.right_motor_torque_limit, 4.40);
        ASSERT_CLOSE(motor_torque.left_motor_torque_limit, 5.20);
    }

    // Test Case 2: Larger Sample Sizes
    {
        TorqueVector<double> tv = {
            .left_torque_vector = 1.5,
            .right_torque_vector = 1.2
        };

        for (double i = 1; i <= 9; i++) {
            CalculateMotorTorque(i, tv, i == 1);
        }
        MotorTorque<double> motor_torque =
            CalculateMotorTorque(10.0, tv);
        ASSERT_CLOSE(motor_torque.right_motor_torque_limit, 8.25);
        ASSERT_CLOSE(motor_torque.left_motor_torque_limit, 6.60);
    }

    // Test Case 3: Sample Size over 10 Entities
    {
        TorqueVector<double> tv = {
            .left_torque_vector = 1.1,
            .right_torque_vector = 1.3
        };

        double new_torque_values[] = {80, 21, 34, 43, 27, 47, 46,
                                      21, 73, 12, 11, 11, 14};
        for (int i = 0; i < 13; i++) {
            CalculateMotorTorque(new_torque_values[i], tv, i == 0);
        }
        MotorTorque<double> motor_torque =
            CalculateMotorTorque(12.0, tv);
        ASSERT_CLOSE(motor_torque.right_motor_torque_limit, 27.4 * 1.1);
        ASSERT_CLOSE(motor_torque.left_motor_torque_limit, 27.4 * 1.2);
    }

    // Test Case 4: Zero values in raw_torque_values
    {
        TorqueVector<double> tv = {
            .left_torque_vector = 1.1,
            .right_torque_vector = 1.2
        };

        double new_torque_values[] = {0.0, 0.0, 0.0};
        for (int i = 0; i < 3; i++) {
            CalculateMotorTorque(new_torque_values[i], tv, i == 0);
        }
        MotorTorque<double> motor_torque = 
            CalculateMotorTorque(0.0, tv);
        ASSERT_CLOSE(motor_torque.right_motor_torque_limit, 0.0);
        ASSERT_CLOSE(motor_torque.left_motor_torque_limit, 0.0);
    }

    // Test Case 5: Single element in raw_torque_values
    {
        TorqueVector<double> tv = {
            .left_torque_vector = 1.5,
            .right_torque_vector = 1.2
        };

        double new_torque_value = 10.0;
        MotorTorque<double> motor_torque =
            CalculateMotorTorque(new_torque_value, tv, true);
        ASSERT_CLOSE(motor_torque.right_motor_torque_limit, 15.00);
        ASSERT_CLOSE(motor_torque.left_motor_torque_limit, 12.00);
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
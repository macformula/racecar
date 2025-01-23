#include <cassert>
#include <iostream>

#include "driver_interface.hpp"
#include "shared/controls/testing.h"

void test_example() {
    ASSERT_TRUE(1 + 1 == 2);

    // assert(1 + 1 == 21);
    // ASSERT_TRUE(1 + 1 == 21);
}

void test_brake_light() {
    // The brake light should be activate if brake pedal pos > 10%
    DriverInterface di;
    {
        DiInput in = {
            .rear_brake_pedal_position = 0.5,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }

    {
        DiInput in = {
            .rear_brake_pedal_position = 0.08,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
    {  // maximum brake pedal position
        DiInput in = {
            .rear_brake_pedal_position = 1.00,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
    {  // Negative brake pedal position
        DiInput in = {
            .rear_brake_pedal_position = -0.08,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
    {  // When brake pedal position = 10%
        DiInput in = {
            .rear_brake_pedal_position = 0.10,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
}

void test_driver_torque_req() {
    // Test: driver_torque_req (Driver Torque Request)
    DriverInterface di;
    {
        // Step 1: INIT -> WAITING_FOR_DRVR
        DiInput in = {.di_cmd = DiCmd::INIT,
                      .rear_brake_pedal_position = 0.0,
                      .front_brake_pedal_position = 0.0,
                      .driver_button = false,
                      .accel_pedal_pos1 = 0.0,
                      .accel_pedal_pos2 = 0.0,
                      .steering_angle = 0.5};
        di.Update(in, 0);

        // Step 2: WAITING_FOR_DRVR -> HV_START_REQ
        in.driver_button = true;
        di.Update(in, 100);

        // Step 3: HV_START_REQ -> MOTOR_START_REQ
        in.di_cmd = DiCmd::HV_ON;
        di.Update(in, 200);

        // Step 4: MOTOR_START_REQ -> RUNNING
        in.di_cmd = DiCmd::READY_TO_DRIVE;
        in.rear_brake_pedal_position = 0.2;
        di.Update(in, 300);

        // Step 5: Request torque in RUNNING state
        in.accel_pedal_pos1 = 25.0;
        in.accel_pedal_pos2 = 25.0;
        DiOutput out = di.Update(in, 3100);

        ASSERT_EQ(out.driver_torque_req, 25);
    }

    // Test zero torque when accelerator is not pressed
    {
        DiInput in = {.di_cmd = DiCmd::INIT,
                      .rear_brake_pedal_position = 0.2,
                      .front_brake_pedal_position = 0.0,
                      .driver_button = false,
                      .accel_pedal_pos1 = 0.0,
                      .accel_pedal_pos2 = 0.0,
                      .steering_angle = 0.1};
        DiOutput out = di.Update(in, 100);
        ASSERT_EQ(out.driver_torque_req,
                  0.0);  // No torque requested in INIT state
    }
}

void test_steering_angle() {
    DriverInterface di;
    {  // invalid steering angle
        DiInput in = {
            .steering_angle = 1.01,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_EQ(out.steering_angle, 0.5);
    }

    {  // valid steering angle
        DiInput in = {
            .steering_angle = 0.5,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_EQ(out.steering_angle, 0.5);
    }
    {  // low limit
        DiInput in = {
            .steering_angle = 0.0,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_EQ(out.steering_angle, 0.0);
    }
}

void test_brake_pedal_position() {
    DriverInterface di;
    {  // break pedal position is 0
        DiInput in = {
            .rear_brake_pedal_position = 0.0,
            .front_brake_pedal_position = 0.0,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_EQ(out.brake_pedal_position, 0.0);
    }
    {  // maximum brake pedal position
        DiInput in = {
            .rear_brake_pedal_position = 0.9f,
            .front_brake_pedal_position = 0.4f,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_EQ(out.brake_pedal_position, 0.9f);
    }
    {  // brake pedal position not in range
        DiInput in = {
            .rear_brake_pedal_position = 0.9f,
            .front_brake_pedal_position = 1.1f,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_EQ(out.brake_pedal_position, 0);
    }
}

void test_driver_speaker() {
    DriverInterface di;
    {
        DiInput in = {.di_cmd = DiCmd::INIT,
                      .rear_brake_pedal_position = 0.2,
                      .front_brake_pedal_position = 0.0,
                      .driver_button = false,
                      .accel_pedal_pos1 = 0.0,
                      .accel_pedal_pos2 = 0.0,
                      .steering_angle = 0.5};
        di.Update(in, 0);

        // Step 2: WAITING_FOR_DRVR -> HV_START_REQ
        in.driver_button = true;
        di.Update(in, 100);

        // Step 3: HV_START_REQ -> MOTOR_START_REQ
        in.di_cmd = DiCmd::HV_ON;
        DiOutput out = di.Update(in, 200);

        // Step 4: MOTOR_START_REQ -> RUNNING
        in.di_cmd = DiCmd::READY_TO_DRIVE;
        out = di.Update(in, 300);

        ASSERT_TRUE(out.driver_speaker);  // Speaker ON when entering RUNNING

        // Test speaker timing
        out = di.Update(in, 2301);  // After 2301ms
        ASSERT_FALSE(out.driver_speaker);

        out = di.Update(in, 2000);  // After 2000ms
        ASSERT_TRUE(out.driver_speaker);
    }
}

int main() {
    test_example();
    test_brake_light();
    test_driver_torque_req();
    test_steering_angle();
    test_brake_pedal_position();
    test_driver_speaker();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
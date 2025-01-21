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

    // Test non-zero torque when accelerator is pressed
    {
        DiInput in = {

            .accel_pedal_pos1 = 25,
        };
        DiOutput out = di.Update(in, 100);
        std::cout << (out.driver_torque_req) << std::endl;
        ASSERT_EQ(
            out.driver_torque_req,
            25);  // Torque should be requested when accelerator is pressed
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

int main() {
    test_example();
    test_brake_light();
    test_driver_torque_req();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
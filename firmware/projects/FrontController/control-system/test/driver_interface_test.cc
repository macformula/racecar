#include "driver_interface.hpp"

#include <cassert>
#include <iostream>

#include "control-system/enums.hpp"
#include "driver_interface_fsm.hpp"
#include "shared/controls/testing.h"

DriverInterface CycleToRunning() {
    DriverInterface di;

    auto out = di.Update({.di_cmd = DiCmd::INIT}, 0);
    ASSERT_EQ(out.di_sts, DiSts::WAITING_FOR_DRVR);

    out = di.Update({.driver_button = true}, 1);
    ASSERT_EQ(out.di_sts, DiSts::HV_START_REQ);

    out = di.Update(
        {
            .di_cmd = DiCmd::HV_ON,
            .driver_button = true,
        },
        2);
    ASSERT_EQ(out.di_sts, DiSts::MOTOR_START_REQ);

    out = di.Update(
        {
            .di_cmd = DiCmd::READY_TO_DRIVE,
            .brake_pedal_pos = 0.2,
        },
        3);
    ASSERT_EQ(out.di_sts, DiSts::RUNNING);

    return di;
}

void test_brake_light() {
    // The brake light should be activate if brake pedal pos > 10%
    DriverInterface di;
    {
        DriverInterface::Input in = {
            .brake_pedal_pos = 0.5,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_TRUE(out.brake_light_en);
        ASSERT_CLOSE(out.brake_pedal_position, 0.5);
    }

    {
        DriverInterface::Input in = {
            .brake_pedal_pos = 0.08,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
        ASSERT_CLOSE(out.brake_pedal_position, 0.08);
    }
    {  // maximum brake pedal position
        DriverInterface::Input in = {
            .brake_pedal_pos = 1.00,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_TRUE(out.brake_light_en);
        ASSERT_CLOSE(out.brake_pedal_position, 1.00);
    }
    {  // Negative brake pedal position
        DriverInterface::Input in = {
            .brake_pedal_pos = -0.08,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
        ASSERT_CLOSE(out.brake_pedal_position, 0);
    }
}

void test_not_ready_drive() {
    DriverInterface di;

    {
        DriverInterface::Output out = di.Update(
            {.accel_pedal_pos1 = 0.50, .accel_pedal_pos2 = 0.50}, 100);

        ASSERT_CLOSE(out.driver_torque_req, 0);
    }
}

void test_driver_torque_req() {
    // Test: driver_torque_req (Driver Torque Request)
    DriverInterface di = CycleToRunning();

    // Test non-zero torque when accelerator is pressed
    {
        DriverInterface::Output out = di.Update(
            {.accel_pedal_pos1 = 0.25, .accel_pedal_pos2 = 0.30}, 100);

        ASSERT_CLOSE(out.driver_torque_req, 0.25);
    }

    // Test zero torque when accelerator is not pressed
    {
        DriverInterface::Output out = di.Update({.accel_pedal_pos1 = 0.0}, 100);
        ASSERT_EQ(out.driver_torque_req, 0.0);
    }
    {  // testing accelerator implausibility
        DriverInterface::Output out = di.Update(
            {.accel_pedal_pos1 = 0.25, .accel_pedal_pos2 = 0.50}, 100);

        ASSERT_CLOSE(out.driver_torque_req, 0);
    }
}

void test_steering_angle() {
    DriverInterface di;
    {  // invalid steering angle
        DriverInterface::Input in = {
            .steering_angle = 1.01,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_CLOSE(out.steering_angle, 0.5);
    }

    {  // valid steering angle
        DriverInterface::Input in = {
            .steering_angle = 0.6,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_CLOSE(out.steering_angle, 0.6);
    }
    {  // low limit
        DriverInterface::Input in = {
            .steering_angle = -0.1,
        };
        DriverInterface::Output out = di.Update(in, 0);
        ASSERT_CLOSE(out.steering_angle, 0.5);
    }
}

void DiTest() {
    test_brake_light();
    test_steering_angle();
    test_driver_torque_req();
    test_not_ready_drive();

    std::cout << "All DI tests passed!" << std::endl;
}
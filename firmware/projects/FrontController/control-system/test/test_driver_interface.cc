#include <gtest/gtest.h>

#include <cassert>

#include "control-system/driver_interface.hpp"
#include "control-system/enums.hpp"

DriverInterface CycleToRunning() {
    DriverInterface di;
    int time = 0;

    auto out = di.Update({.di_cmd = DiCmd::INIT}, time++);
    assert(out.di_sts == DiSts::WAITING_FOR_DRIVER_HV);

    out = di.Update({.driver_button = true}, time++);
    assert(out.di_sts == DiSts::REQUESTED_HV_START);

    out = di.Update(
        {
            .di_cmd = DiCmd::HV_IS_ON,
            .driver_button = true,
        },
        time++);
    assert(out.di_sts == DiSts::WAITING_FOR_DRIVER_MOTOR);

    out = di.Update(
        {
            .di_cmd = DiCmd::HV_IS_ON,
            .driver_button = true,
        },
        time++);
    assert(out.di_sts == DiSts::REQUESTED_MOTOR_START);

    out = di.Update(
        {
            .di_cmd = DiCmd::READY_TO_DRIVE,
            .brake_pedal_pos = 0.2,
        },
        time++);
    assert(out.di_sts == DiSts::RUNNING);

    return di;
}

TEST(DriverInterface, BrakeLight) {
    // The brake light should be activate if brake pedal pos > 10%
    DriverInterface di;
    {
        DriverInterface::Input in = {
            .brake_pedal_pos = 0.5,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_TRUE(out.brake_light_en);
        EXPECT_FLOAT_EQ(out.brake_pedal_position, 0.5);
    }

    {
        DriverInterface::Input in = {
            .brake_pedal_pos = 0.08,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_FALSE(out.brake_light_en);
        EXPECT_FLOAT_EQ(out.brake_pedal_position, 0.08);
    }
    {  // maximum brake pedal position
        DriverInterface::Input in = {
            .brake_pedal_pos = 1.00,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_TRUE(out.brake_light_en);
        EXPECT_FLOAT_EQ(out.brake_pedal_position, 1.00);
    }
    {  // Negative brake pedal position
        DriverInterface::Input in = {
            .brake_pedal_pos = -0.08,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_FALSE(out.brake_light_en);
        EXPECT_FLOAT_EQ(out.brake_pedal_position, 0);
    }
}

TEST(DriverInterface, NotReadyToDrive) {
    DriverInterface di;

    {
        DriverInterface::Output out = di.Update(
            {.accel_pedal_pos1 = 0.50, .accel_pedal_pos2 = 0.50}, 100);

        EXPECT_FLOAT_EQ(out.driver_torque_req, 0);
    }
}

TEST(DriverInterface, TorqueRequest) {
    // Test: driver_torque_req (Driver Torque Request)
    DriverInterface di = CycleToRunning();

    // Test non-zero torque when accelerator is pressed
    {
        DriverInterface::Output out = di.Update(
            {.accel_pedal_pos1 = 0.25, .accel_pedal_pos2 = 0.30}, 100);

        EXPECT_FLOAT_EQ(out.driver_torque_req, 0.25);
    }

    // Test zero torque when accelerator is not pressed
    {
        DriverInterface::Output out = di.Update({.accel_pedal_pos1 = 0.0}, 100);
        EXPECT_FLOAT_EQ(out.driver_torque_req, 0.0);
    }
    {  // testing accelerator implausibility
        DriverInterface::Output out = di.Update(
            {.accel_pedal_pos1 = 0.25, .accel_pedal_pos2 = 0.50}, 100);

        EXPECT_FLOAT_EQ(out.driver_torque_req, 0);
    }
}
TEST(DriverInterface, SteeringAngle) {
    DriverInterface di;
    {  // invalid steering angle
        DriverInterface::Input in = {
            .steering_angle = 1.01,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_FLOAT_EQ(out.steering_angle, 0.5);
    }

    {  // valid steering angle
        DriverInterface::Input in = {
            .steering_angle = 0.6,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_FLOAT_EQ(out.steering_angle, 0.6);
    }
    {  // low limit
        DriverInterface::Input in = {
            .steering_angle = -0.1,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_FLOAT_EQ(out.steering_angle, 0.5);
    }
}
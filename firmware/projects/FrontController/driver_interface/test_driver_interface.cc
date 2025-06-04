#include <gtest/gtest.h>

#include <cassert>

#include "../enums.hpp"
#include "../generated/can/veh_messages.hpp"
#include "driver_interface.hpp"

using DashState = generated::can::RxDashboardStatus::DashState_t;

DriverInterface CycleToState(const DiSts start_state) {
    using enum DiSts;
    auto supported_states = {
        IDLE,
        WAITING_FOR_DRIVER_HV,
        REQUESTED_HV_START,
        WAITING_FOR_DRIVER_MOTOR,
        REQUESTED_MOTOR_START,
        RUNNING,
        ERR,
        ERR_COASTING,
    };

    bool is_supported = false;
    for (auto s : supported_states) {
        if (start_state == s) {
            is_supported = true;
        }
    }
    assert(is_supported);

    DriverInterface di;
    int time = 0;
    if (start_state == IDLE) return di;

    auto out = di.Update({.command = DiCmd::INIT}, time++);
    assert(out.status == WAITING_FOR_DRIVER_HV);
    if (start_state == WAITING_FOR_DRIVER_HV) return di;

    out = di.Update({.dash_cmd = DashState::STARTING_HV}, time++);
    assert(out.status == REQUESTED_HV_START);
    if (start_state == REQUESTED_HV_START) return di;

    out = di.Update({.command = DiCmd::HV_IS_ON}, time++);
    assert(out.status == WAITING_FOR_DRIVER_MOTOR);
    if (start_state == WAITING_FOR_DRIVER_MOTOR) return di;

    out = di.Update({.dash_cmd = DashState::STARTING_MOTORS}, time++);
    assert(out.status == REQUESTED_MOTOR_START);
    if (start_state == REQUESTED_MOTOR_START) return di;

    out = di.Update(
        {
            .command = DiCmd::READY_TO_DRIVE,
            .brake_pedal_pos = 20,
        },
        time++);
    assert(out.status == RUNNING);
    if (start_state == RUNNING) return di;

    if (start_state == ERR) {
        out = di.Update(
            {
                .command = DiCmd::READY_TO_DRIVE,
                .accel_pedal_pos1 = 0,
                .accel_pedal_pos2 = 100,  // should force implausible
            },
            time++);
        assert(out.status == ERR);
        if (start_state == ERR) return di;
    } else if (start_state == ERR_COASTING) {
        out = di.Update(
            {
                .command = DiCmd::RUN_ERROR,
            },
            time++);
        assert(out.status == ERR_COASTING);
        if (start_state == ERR_COASTING) return di;
    }

    assert(false);  // shouldn't get here if is_supported(state)
}

TEST(DriverInterface, BrakeLight) {
    // The brake light should be activate if brake pedal pos > 10%
    DriverInterface di;
    {
        DriverInterface::Input in = {
            .brake_pedal_pos = 50,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_TRUE(out.brake_light_en);
    }

    {
        DriverInterface::Input in = {
            .brake_pedal_pos = 8,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_FALSE(out.brake_light_en);
    }
    {  // maximum brake pedal position
        DriverInterface::Input in = {
            .brake_pedal_pos = 100,
        };
        DriverInterface::Output out = di.Update(in, 0);
        EXPECT_TRUE(out.brake_light_en);
    }
}

TEST(DriverInterface, NotReadyToDrive) {
    DriverInterface di;

    {
        DriverInterface::Output out =
            di.Update({.accel_pedal_pos1 = 50, .accel_pedal_pos2 = 50}, 100);

        EXPECT_FLOAT_EQ(out.driver_torque_req, 0);
    }
}

TEST(DriverInterface, TorqueRequest) {
    // Test: driver_torque_req (Driver Torque Request)
    DriverInterface di = CycleToState(DiSts::RUNNING);

    // Test non-zero torque when accelerator is pressed
    {
        DriverInterface::Output out =
            di.Update({.accel_pedal_pos1 = 25, .accel_pedal_pos2 = 30}, 100);

        EXPECT_FLOAT_EQ(out.driver_torque_req, 25);
    }

    // Test zero torque when accelerator is not pressed
    {
        DriverInterface::Output out = di.Update({.accel_pedal_pos1 = 0}, 100);
        EXPECT_FLOAT_EQ(out.driver_torque_req, 0);
    }
    {  // testing accelerator implausibility
        DriverInterface::Output out =
            di.Update({.accel_pedal_pos1 = 25, .accel_pedal_pos2 = 50}, 100);

        EXPECT_FLOAT_EQ(out.driver_torque_req, 0);
    }
}

TEST(DriverInterfaceFsm, Sequence) {
    DriverInterface di;
    DriverInterface::Output out;

    int time_ms = 0;
    DriverInterface::Input in{
        .command = DiCmd::HV_IS_ON,  // not init
        .brake_pedal_pos = 0,
        .dash_cmd = DashState::LOGO,
        .accel_pedal_pos1 = 0,
        .accel_pedal_pos2 = 0,
    };

    {  // stay in INIT until governor commands INIT
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::IDLE);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.driver_speaker);

        in.command = DiCmd::INIT;

        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_HV);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.driver_speaker);
    }

    {  // Start the motor only if the high-voltage is on and the driver presses
        // the button

        // Initiale the sequence from the Dashboard
        in.dash_cmd = DashState::STARTING_HV;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_HV_START);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.driver_speaker);

        in.command = DiCmd::HV_IS_ON;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_MOTOR);

        in.dash_cmd = DashState::PRESS_FOR_MOTOR;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_MOTOR)
            << "Should wait for dashboard button.";

        in.dash_cmd = DashState::STARTING_MOTORS;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.driver_speaker);
    }

    {  // Only enter RUNNING when both command and pedal are valid
        in.brake_pedal_pos = 0;
        in.command = DiCmd::HV_IS_ON;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);

        in.command = DiCmd::HV_IS_ON;
        in.brake_pedal_pos = 5;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);

        in.command = DiCmd::READY_TO_DRIVE;
        in.brake_pedal_pos = 0;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);

        in.command = DiCmd::READY_TO_DRIVE;
        in.brake_pedal_pos = 50;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::RUNNING);
        EXPECT_TRUE(out.ready_to_drive);
        EXPECT_TRUE(out.driver_speaker);
    }

    {  // roughly test speaker (see test_start_running() for more detail)
        time_ms += 1900;

        out = di.Update(in, time_ms);
        EXPECT_TRUE(out.driver_speaker);
        EXPECT_TRUE(out.ready_to_drive);

        time_ms += 200;  // 2.1s since RUNNING

        out = di.Update(in, time_ms);
        EXPECT_FALSE(out.driver_speaker);
        EXPECT_TRUE(out.ready_to_drive);
    }

    {  // shutdown
        in.command = DiCmd::SHUTDOWN;
        out = di.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::IDLE);
    }
}

TEST(DriverInterfaceFsm, StartRunning) {
    DriverInterface di = CycleToState(DiSts::REQUESTED_MOTOR_START);

    DriverInterface::Input in{
        .command = DiCmd::HV_IS_ON,
        .brake_pedal_pos = 50,  // brakes pressed
        .dash_cmd = DashState::BRAKE_TO_START,
        .accel_pedal_pos1 = 0,
        .accel_pedal_pos2 = 0,
    };
    DriverInterface::Output out;

    int time_ms = 0;
    for (; time_ms < 10; time_ms++) {
        out = di.Update(in, time_ms);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.driver_speaker);
    }

    // Governor replies RTD
    in.command = DiCmd::READY_TO_DRIVE;
    for (; time_ms < 5000; time_ms++) {
        out = di.Update(in, time_ms);
        EXPECT_TRUE(out.ready_to_drive);

        // Speaker should buzz for 2 seconds (note 10 ms elapsed earlier)
        if (time_ms <= 2010) {
            EXPECT_TRUE(out.driver_speaker);
        } else {
            EXPECT_FALSE(out.driver_speaker);
        }
    }
}

TEST(DriverInterfaceFsm, GeneralError) {
    for (auto should_err : {
             DiSts::WAITING_FOR_DRIVER_HV, DiSts::REQUESTED_HV_START,
             DiSts::REQUESTED_MOTOR_START, DiSts::RUNNING,
             DiSts::ERR,  // we should just stay in this state
         }) {
        DriverInterface di = CycleToState(should_err);
        auto out =
            di.Update({.accel_pedal_pos1 = 0, .accel_pedal_pos2 = 100}, 0);
        ASSERT_EQ(out.status, DiSts::ERR);
    }

    for (auto shouldnt_err : {DiSts::IDLE, DiSts::ERR_COASTING}) {
        DriverInterface di = CycleToState(shouldnt_err);
        auto out =
            di.Update({.accel_pedal_pos1 = 0, .accel_pedal_pos2 = 100}, 0);
        ASSERT_NE(out.status, DiSts::ERR);
    }
}

TEST(DriverInterfaceFsm, RunningError) {
    DriverInterface di = CycleToState(DiSts::RUNNING);
    auto out = di.Update({.command = DiCmd::RUN_ERROR}, 0);
    ASSERT_EQ(out.status, DiSts::ERR_COASTING);

    // Assert no other states go to ERR_COASTING
    for (auto state : {
             DiSts::IDLE,
             DiSts::WAITING_FOR_DRIVER_HV,
             DiSts::REQUESTED_HV_START,
             DiSts::REQUESTED_MOTOR_START,
             DiSts::ERR,
         }) {
        DriverInterface not_running = CycleToState(state);
        auto out = not_running.Update({.command = DiCmd::RUN_ERROR}, 0);
        ASSERT_NE(out.status, DiSts::ERR_COASTING);
    }
}

TEST(DriverInterfaceFsm, Shutdown) {
    // Should shutdown from the running states but not the error
    for (auto state : {DiSts::WAITING_FOR_DRIVER_HV, DiSts::REQUESTED_HV_START,
                       DiSts::REQUESTED_MOTOR_START, DiSts::RUNNING}) {
        DriverInterface di = CycleToState(state);
        auto out = di.Update({.command = DiCmd::SHUTDOWN}, 0);
        ASSERT_EQ(out.status, DiSts::IDLE);
    }

    for (auto state : {DiSts::ERR, DiSts::ERR_COASTING}) {
        DriverInterface di = CycleToState(state);
        auto out = di.Update({.command = DiCmd::SHUTDOWN}, 0);
        ASSERT_NE(out.status, DiSts::IDLE);
    }
}

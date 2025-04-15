#include "control-system/driver_interface_fsm.hpp"
#include "gtest/gtest.h"

TEST(DriverInterfaceFsm, Sequence) {
    DiFsm fsm{};
    DiFsm::Output out;

    int time_ms = 0;
    DiFsm::Input in{
        .command = DiCmd::HV_IS_ON,  // not init
        .driver_button = false,
        .brake_pedal_pos = 0.,
        .di_error = false,
    };

    {  // stay in INIT until governor commands INIT
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::IDLE);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.speaker_enable);

        in.command = DiCmd::INIT;

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_HV);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.speaker_enable);
    }

    {  // Nothing should happen if button isn't pressed
        in.driver_button = false;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_HV);
    }

    {  // Start the motor only if the high-voltage is on and the driver presses
       // the button
        in.driver_button = true;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_HV_START);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.speaker_enable);

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_HV_START);

        in.command = DiCmd::HV_IS_ON;
        in.driver_button = false;

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_MOTOR);

        in.driver_button = false;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRIVER_MOTOR)
            << "Should wait for button.";

        in.driver_button = true;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.speaker_enable);
    }

    {  // Only enter RUNNING when both command and pedal are valid
        in.brake_pedal_pos = 0.0;
        in.command = DiCmd::HV_IS_ON;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);

        in.command = DiCmd::HV_IS_ON;
        in.brake_pedal_pos = 0.5;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);

        in.command = DiCmd::READY_TO_DRIVE;
        in.brake_pedal_pos = 0.0;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::REQUESTED_MOTOR_START);

        in.command = DiCmd::READY_TO_DRIVE;
        in.brake_pedal_pos = 0.5;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::RUNNING);
        EXPECT_TRUE(out.ready_to_drive);
        EXPECT_TRUE(out.speaker_enable);
    }

    {  // roughly test speaker (see test_start_running() for more detail)
        time_ms += 1900;

        out = fsm.Update(in, time_ms);
        EXPECT_TRUE(out.speaker_enable);
        EXPECT_TRUE(out.ready_to_drive);

        time_ms += 200;  // 2.1s since RUNNING

        out = fsm.Update(in, time_ms);
        EXPECT_FALSE(out.speaker_enable);
        EXPECT_TRUE(out.ready_to_drive);
    }

    {  // shutdown
        in.command = DiCmd::SHUTDOWN;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::IDLE);
    }
}

TEST(DriverInterfaceFsm, StartRunning) {
    DiFsm fsm{DiSts::REQUESTED_MOTOR_START};

    DiFsm::Input in{
        .command = DiCmd::HV_IS_ON,
        .driver_button = false,
        .brake_pedal_pos = 0.5,  // brakes pressed
        .di_error = false,
    };
    DiFsm::Output out;

    int time_ms = 0;
    for (; time_ms < 10; time_ms++) {
        out = fsm.Update(in, time_ms);
        EXPECT_FALSE(out.ready_to_drive);
        EXPECT_FALSE(out.speaker_enable);
    }

    // Governor replies RTD
    in.command = DiCmd::READY_TO_DRIVE;
    for (; time_ms < 5000; time_ms++) {
        out = fsm.Update(in, time_ms);
        EXPECT_TRUE(out.ready_to_drive);

        // Speaker should buzz for 2 seconds (note 10 ms elapsed earlier)
        if (time_ms <= 2010) {
            EXPECT_TRUE(out.speaker_enable);
        } else {
            EXPECT_FALSE(out.speaker_enable);
        }
    }
}

TEST(DriverInterfaceFsm, GeneralError) {
    for (auto should_err : {
             DiSts::WAITING_FOR_DRIVER_HV, DiSts::REQUESTED_HV_START,
             DiSts::REQUESTED_MOTOR_START, DiSts::RUNNING,
             DiSts::ERR,  // we should just stay in this state
         }) {
        DiFsm fsm{should_err};
        auto out = fsm.Update({.di_error = true}, 0);
        ASSERT_EQ(out.status, DiSts::ERR);
    }

    for (auto shouldnt_err : {DiSts::IDLE, DiSts::ERR_COASTING}) {
        DiFsm fsm{shouldnt_err};
        auto out = fsm.Update({.di_error = true}, 0);
        ASSERT_NE(out.status, DiSts::ERR);
    }
}

TEST(DriverInterfaceFsm, RunningError) {
    DiFsm running{DiSts::RUNNING};
    auto out = running.Update({.command = DiCmd::RUN_ERROR}, 0);
    ASSERT_EQ(out.status, DiSts::ERR_COASTING);

    // Assert no other states go to ERR_COASTING
    for (auto state : {
             DiSts::IDLE,
             DiSts::WAITING_FOR_DRIVER_HV,
             DiSts::REQUESTED_HV_START,
             DiSts::REQUESTED_MOTOR_START,
             DiSts::ERR,
         }) {
        DiFsm not_running{state};
        auto out = not_running.Update({.command = DiCmd::RUN_ERROR}, 0);
        ASSERT_NE(out.status, DiSts::ERR_COASTING);
    }
}

TEST(DriverInterfaceFsm, Shutdown) {
    // Should shutdown from the running states but not the error
    for (auto state : {DiSts::WAITING_FOR_DRIVER_HV, DiSts::REQUESTED_HV_START,
                       DiSts::REQUESTED_MOTOR_START, DiSts::RUNNING}) {
        DiFsm fsm{state};
        auto out = fsm.Update({.command = DiCmd::SHUTDOWN}, 0);
        ASSERT_EQ(out.status, DiSts::IDLE);
    }

    for (auto state : {DiSts::ERR, DiSts::ERR_COASTING}) {
        DiFsm fsm{state};
        auto out = fsm.Update({.command = DiCmd::SHUTDOWN}, 0);
        ASSERT_NE(out.status, DiSts::IDLE);
    }
}

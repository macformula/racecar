#include <iostream>

#include "driver_interface_fsm.hpp"
#include "testing.h"

void test_sequence() {
    DiFsm fsm{};
    DiFsm::Output out;

    int time_ms = 0;
    DiFsm::Input in{
        .command = DiCmd::UNKNOWN,
        .driver_button = false,
        .brake_pedal_pos = 0.,
        .di_error = false,
    };

    {  // stay in INIT until governor commands INIT
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::INIT);
        ASSERT_FALSE(out.ready_to_drive);
        ASSERT_FALSE(out.speaker_enable);

        in.command = DiCmd::INIT;

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRVR);
        ASSERT_FALSE(out.ready_to_drive);
        ASSERT_FALSE(out.speaker_enable);
    }

    {  // Nothing should happen if button isn't pressed
        in.driver_button = false;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::WAITING_FOR_DRVR);
    }

    {  // Start the motor only if the high-voltage is on and the driver presses
       // the button
        in.driver_button = true;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::HV_START_REQ);
        ASSERT_FALSE(out.ready_to_drive);
        ASSERT_FALSE(out.speaker_enable);

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::HV_START_REQ);

        in.command = DiCmd::HV_ON;
        in.driver_button = false;

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::HV_START_REQ);

        in.command = DiCmd::HV_ON;
        in.driver_button = true;

        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::MOTOR_START_REQ);
        ASSERT_FALSE(out.ready_to_drive);
        ASSERT_FALSE(out.speaker_enable);
    }

    {  // Only enter RUNNING when both command and pedal are valid
        in.brake_pedal_pos = 0.0;
        in.command = DiCmd::HV_ON;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::MOTOR_START_REQ);

        in.command = DiCmd::HV_ON;
        in.brake_pedal_pos = 0.5;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::MOTOR_START_REQ);

        in.command = DiCmd::READY_TO_DRIVE;
        in.brake_pedal_pos = 0.0;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::MOTOR_START_REQ);

        in.command = DiCmd::READY_TO_DRIVE;
        in.brake_pedal_pos = 0.5;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::RUNNING);
        ASSERT_TRUE(out.ready_to_drive);
        ASSERT_TRUE(out.speaker_enable);
    }

    {  // roughly test speaker (see test_start_running() for more detail)
        time_ms += 1900;

        out = fsm.Update(in, time_ms);
        ASSERT_TRUE(out.speaker_enable);
        ASSERT_TRUE(out.ready_to_drive);

        time_ms += 200;  // 2.1s since RUNNING

        out = fsm.Update(in, time_ms);
        ASSERT_FALSE(out.speaker_enable);
        ASSERT_TRUE(out.ready_to_drive);
    }

    {  // shutdown
        in.command = DiCmd::SHUTDOWN;
        out = fsm.Update(in, time_ms++);
        ASSERT_EQ(out.status, DiSts::INIT);
    }

    std::cout << "Passed test_sequence" << std::endl;
}

void test_start_running() {
    DiFsm fsm{DiSts::MOTOR_START_REQ};

    DiFsm::Input in{
        .command = DiCmd::HV_ON,
        .driver_button = false,
        .brake_pedal_pos = 0.5,  // brakes pressed
        .di_error = false,
    };
    DiFsm::Output out;

    int time_ms = 0;
    for (; time_ms < 10; time_ms++) {
        out = fsm.Update(in, time_ms);
        ASSERT_FALSE(out.ready_to_drive);
        ASSERT_FALSE(out.speaker_enable);
    }

    // Governor replies RTD
    in.command = DiCmd::READY_TO_DRIVE;
    for (; time_ms < 5000; time_ms++) {
        out = fsm.Update(in, time_ms);
        ASSERT_TRUE(out.ready_to_drive);

        // Speaker should buzz for 2 seconds (note 10 ms elapsed earlier)
        if (time_ms <= 2010) {
            ASSERT_TRUE(out.speaker_enable);
        } else {
            ASSERT_FALSE(out.speaker_enable);
        }
    }

    std::cout << "Passed test_start_running" << std::endl;
}

void test_general_error() {
    for (auto should_err : {
             DiSts::WAITING_FOR_DRVR, DiSts::HV_START_REQ,
             DiSts::MOTOR_START_REQ, DiSts::RUNNING,
             DiSts::ERR,  // we should just stay in this state
         }) {
        DiFsm fsm{should_err};
        auto out = fsm.Update({.di_error = true}, 0);
        ASSERT_EQ(out.status, DiSts::ERR);
    }

    for (auto shouldnt_err : {DiSts::INIT, DiSts::ERR_COASTING}) {
        DiFsm fsm{shouldnt_err};
        auto out = fsm.Update({.di_error = true}, 0);
        ASSERT_NEQ(out.status, DiSts::ERR);
    }

    std::cout << "Passed test_general_error" << std::endl;
}

void test_running_error() {
    DiFsm running{DiSts::RUNNING};
    auto out = running.Update({.command = DiCmd::RUN_ERROR}, 0);
    ASSERT_EQ(out.status, DiSts::ERR_COASTING);

    // Assert no other states go to ERR_COASTING
    for (auto state : {
             DiSts::INIT,
             DiSts::WAITING_FOR_DRVR,
             DiSts::HV_START_REQ,
             DiSts::MOTOR_START_REQ,
             DiSts::ERR,
         }) {
        DiFsm not_running{state};
        auto out = not_running.Update({.command = DiCmd::RUN_ERROR}, 0);
        ASSERT_NEQ(out.status, DiSts::ERR_COASTING);
    }

    std::cout << "Passed test_running_error" << std::endl;
}

void test_shutdown() {
    // Should shutdown from the running states but not the error
    for (auto state : {DiSts::WAITING_FOR_DRVR, DiSts::HV_START_REQ,
                       DiSts::MOTOR_START_REQ, DiSts::RUNNING}) {
        DiFsm fsm{state};
        auto out = fsm.Update({.command = DiCmd::SHUTDOWN}, 0);
        ASSERT_EQ(out.status, DiSts::INIT);
    }

    for (auto state : {DiSts::ERR, DiSts::ERR_COASTING}) {
        DiFsm fsm{state};
        auto out = fsm.Update({.command = DiCmd::SHUTDOWN}, 0);
        ASSERT_NEQ(out.status, DiSts::INIT);
    }

    std::cout << "Passed test_shutdown" << std::endl;
}

int main() {
    test_sequence();
    test_start_running();
    test_running_error();
    test_general_error();
    test_shutdown();

    std::cout << "All tests passed!" << std::endl;
}
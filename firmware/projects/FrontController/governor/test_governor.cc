#include "../enums.hpp"
#include "governor.hpp"
#include "gtest/gtest.h"

/// Helper function to prepare a Governor in a specific state for testing.
Governor CycleToState(GovSts desired_state_) {
    using enum GovSts;

    // Only the standard operation path states are supported
    auto possible_states = {STARTUP_HV,    STARTUP_READY_TO_DRIVE,
                            STARTUP_MOTOR, STARTUP_SEND_READY_TO_DRIVE,
                            RUNNING,       SHUTDOWN};
    bool desired_state_is_supported = false;
    for (auto state : possible_states) {
        if (desired_state_ == state) {
            desired_state_is_supported = true;
            break;
        }
    }
    assert(desired_state_is_supported);

    Governor g;
    Governor::Input in{
        .acc_sts = AccSts::INIT,
        .mi_sts = MiSts::INIT,
        .di_sts = DiSts::IDLE,
    };

    int time = 0;

    g.Update(in, time);
    time += 2000;

    in.di_sts = DiSts::REQUESTED_HV_START;
    assert(g.Update(in, ++time).gov_sts == STARTUP_HV);
    if (desired_state_ == STARTUP_HV) return g;

    in.acc_sts = AccSts::RUNNING;
    assert(g.Update(in, ++time).gov_sts == STARTUP_READY_TO_DRIVE);
    if (desired_state_ == STARTUP_READY_TO_DRIVE) return g;

    in.di_sts = DiSts::REQUESTED_MOTOR_START;
    assert(g.Update(in, ++time).gov_sts == STARTUP_MOTOR);
    if (desired_state_ == STARTUP_MOTOR) return g;

    in.mi_sts = MiSts::RUNNING;
    assert(g.Update(in, ++time).gov_sts == STARTUP_SEND_READY_TO_DRIVE);
    if (desired_state_ == STARTUP_SEND_READY_TO_DRIVE) return g;

    in.di_sts = DiSts::RUNNING;
    assert(g.Update(in, ++time).gov_sts == RUNNING);
    if (desired_state_ == RUNNING) return g;

    // HVIL is not part of BM anymore. what should this be?
    // in.bm_sts = BmSts::HVIL_INTERRUPT;
    // assert(g.Update(in, ++time).gov_sts == SHUTDOWN);
    // if (desired_state_ == SHUTDOWN) return g;

    // We shouldn't get this far if desired_state is possible
    assert(false);
}

TEST(Governor, NormalSequence) {
    Governor g;

    Governor::Input in{
        .acc_sts = AccSts::INIT,
        .mi_sts = MiSts::INIT,
        .di_sts = DiSts::IDLE,
    };

    int time = 0;

    {
        auto out = g.Update(in, time);
        ASSERT_EQ(out.gov_sts, GovSts::INIT);
        EXPECT_EQ(out.di_cmd, driver_interface::Command::INIT);
        EXPECT_EQ(out.acc_cmd, accumulator::Command::INIT);
        EXPECT_EQ(out.mi_cmd, motor::Command::INIT);
    }

    in.di_sts = DiSts::REQUESTED_HV_START;
    time += 1999;

    {  // Shouldn't go yet
        auto out = g.Update(in, time);
        ASSERT_EQ(out.gov_sts, GovSts::INIT);
    }

    time += 1;

    {  // 2s have elapsed -> enter startup
        auto out = g.Update(in, time);
        ASSERT_EQ(out.gov_sts, GovSts::STARTUP_HV);
        EXPECT_EQ(out.acc_cmd, accumulator::Command::STARTUP);

        ASSERT_EQ(g.Update(in, ++time).gov_sts, GovSts::STARTUP_HV);
    }

    in.acc_sts = AccSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, GovSts::STARTUP_READY_TO_DRIVE);
        EXPECT_EQ(out.di_cmd, driver_interface::Command::HV_IS_ON);

        ASSERT_EQ(g.Update(in, ++time).gov_sts, GovSts::STARTUP_READY_TO_DRIVE);
    }

    in.di_sts = DiSts::REQUESTED_MOTOR_START;

    {
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, GovSts::STARTUP_MOTOR);
        EXPECT_EQ(out.mi_cmd, motor::Command::STARTUP);

        ASSERT_EQ(g.Update(in, ++time).gov_sts, GovSts::STARTUP_MOTOR);
    }

    in.mi_sts = MiSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, GovSts::STARTUP_SEND_READY_TO_DRIVE);
        EXPECT_EQ(out.di_cmd, driver_interface::Command::READY_TO_DRIVE);

        assert(g.Update(in, ++time).gov_sts ==
               GovSts::STARTUP_SEND_READY_TO_DRIVE);
    }

    in.di_sts = DiSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, GovSts::RUNNING);
    }
}

// HVIL not part of BM anymore
// TEST(Governor, ShutdownHvil) {
//     Governor g = CycleToState(GovSts::RUNNING);

//     Governor::Input in{
//         .bm_sts = BmSts::RUNNING,
//         .mi_sts = MiSts::RUNNING,
//         .di_sts = DiSts::RUNNING,
//     };

//     int time = 5000;
//     ASSERT_EQ(g.Update(in, time).gov_sts ,  GovSts::RUNNING);

//     in.bm_sts = BmSts::HVIL_INTERRUPT;

//     {
//         auto out = g.Update(in, ++time);
//         ASSERT_EQ(out.gov_sts ,  GovSts::SHUTDOWN);
//         EXPECT_EQ(out.mi_cmd ,  motor::Command::SHUTDOWN);
//         EXPECT_EQ(out.di_cmd ,  driver_interface::Command::SHUTDOWN);

//         ASSERT_EQ(g.Update(in, ++time).gov_sts ,  GovSts::SHUTDOWN);
//     }

//     in.bm_sts = BmSts::INIT;

//     {
//         auto out = g.Update(in, ++time);
//         ASSERT_EQ(out.gov_sts ,  GovSts::INIT);
//         EXPECT_EQ(out.bm_cmd ,  BmCmd::INIT);
//         EXPECT_EQ(out.di_cmd ,  driver_interface::Command::INIT);
//     }

//     std::cout << "Passed test_shutdown_hvil!" << std::endl;
// }

TEST(Governor, LowSocShutdown) {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .acc_sts = AccSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };

    int time = 5000;
    ASSERT_EQ(g.Update(in, time).gov_sts, GovSts::RUNNING);

    in.acc_sts = AccSts::LOW_SOC;

    {
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, GovSts::SHUTDOWN);
        EXPECT_EQ(out.mi_cmd, motor::Command::SHUTDOWN);
        EXPECT_EQ(out.di_cmd, driver_interface::Command::SHUTDOWN);

        ASSERT_EQ(g.Update(in, ++time).gov_sts, GovSts::SHUTDOWN);
    }

    in.acc_sts = AccSts::INIT;

    {
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, GovSts::INIT);
        EXPECT_EQ(out.acc_cmd, accumulator::Command::INIT);
        EXPECT_EQ(out.di_cmd, driver_interface::Command::INIT);
    }
}

TEST(Governor, RunningError) {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .acc_sts = AccSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };
    int time = 0;

    ASSERT_EQ(g.Update(in, ++time).gov_sts, GovSts::RUNNING);

    in.acc_sts = AccSts::ERR_RUNNING;

    auto out = g.Update(in, ++time);
    ASSERT_EQ(out.gov_sts, GovSts::ERR_RUNNING_HV);
    EXPECT_EQ(out.di_cmd, driver_interface::Command::RUN_ERROR);
}

TEST(Governor, RunningMotorError) {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .acc_sts = AccSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };
    int time = 0;

    ASSERT_EQ(g.Update(in, ++time).gov_sts, GovSts::RUNNING);

    in.mi_sts = MiSts::ERR;

    auto out = g.Update(in, ++time);
    ASSERT_EQ(out.gov_sts, GovSts::ERR_RUNNING_MOTOR);
    EXPECT_EQ(out.di_cmd, driver_interface::Command::RUN_ERROR);
}

TEST(Governor, StartupDriverError) {
    for (auto start_state : {
             GovSts::STARTUP_HV,
             GovSts::STARTUP_READY_TO_DRIVE,
             GovSts::STARTUP_MOTOR,
             GovSts::STARTUP_SEND_READY_TO_DRIVE,
         }) {
        Governor g = CycleToState(start_state);

        Governor::Input in{
            .acc_sts = AccSts::INIT,
            .mi_sts = MiSts::INIT,
            .di_sts = DiSts::IDLE,
        };

        ASSERT_EQ(g.Update(in, 0).gov_sts, start_state);

        in.di_sts = DiSts::ERR;

        auto out = g.Update(in, 1);

        ASSERT_EQ(out.gov_sts, GovSts::ERR_STARTUP_DI);
    }
}

// hvil is not part of BM anymore
// TEST(Governor, StartupHvError) {
//     for (auto start_state : {
//              GovSts::STARTUP_HV,
//              GovSts::STARTUP_READY_TO_DRIVE,
//              GovSts::STARTUP_MOTOR,
//              GovSts::STARTUP_SEND_READY_TO_DRIVE,
//          }) {
//         Governor g = CycleToState(start_state);

//         Governor::Input in{
//             .bm_sts = BmSts::INIT,
//             .mi_sts = MiSts::INIT,
//             .di_sts = DiSts::IDLE,
//         };

//         ASSERT_EQ(g.Update(in, 0).gov_sts ,  start_state);

//         in.bm_sts = BmSts::HVIL_INTERRUPT;

//         {
//             auto out = g.Update(in, 1);
//             ASSERT_EQ(out.gov_sts ,  GovSts::ERR_STARTUP_HV);
//             EXPECT_EQ(out.di_cmd ,  driver_interface::Command::SHUTDOWN);
//         }

//         // Return to INIT only after BM=INIT
//         ASSERT_EQ(g.Update(in, 2).gov_sts ,  GovSts::ERR_STARTUP_HV);

//         in.bm_sts = BmSts::INIT;

//         {
//             auto out = g.Update(in, 3);
//             ASSERT_EQ(out.gov_sts ,  GovSts::INIT);
//             EXPECT_EQ(out.di_cmd ,  driver_interface::Command::INIT);
//             EXPECT_EQ(out.bm_cmd ,  BmCmd::INIT);
//         }
//     }
// }

TEST(Governor, StartupMotorError) {
    // fail the motors 6 times. should go through reset until the last time
    // this tests both the ERR_STARTUP_MOTOR_FAULT and ERR_STARTUP_MOTOR_RESET
    // states
    using enum GovSts;

    const int kExpectedMotorAttempts = 5;  // should match Governor constant

    // Create a state-string tuple to print during test
    std::vector<std::tuple<GovSts, std::string>> start_states = {
        {STARTUP_HV, "STARTUP_HV"},
        {STARTUP_READY_TO_DRIVE, "STARTUP_READY_TO_DRIVE"},
        {STARTUP_MOTOR, "STARTUP_MOTOR"},
        {STARTUP_SEND_READY_TO_DRIVE, "STARTUP_SEND_READY_TO_DRIVE"},
    };

    for (auto [start_state, str] : start_states) {
        Governor g = CycleToState(start_state);

        // Must reduce the number of attempts if CycleToState already
        // incremented motor_start_count_
        int cycled_motor_starts = 0;
        if (start_state == STARTUP_MOTOR ||
            start_state == STARTUP_SEND_READY_TO_DRIVE) {
            cycled_motor_starts = 1;
        }

        Governor::Input in{
            .acc_sts = AccSts::INIT,
            .mi_sts = MiSts::INIT,
            .di_sts = DiSts::REQUESTED_HV_START,
        };
        int time = 0;

        ASSERT_EQ(g.Update(in, time).gov_sts, start_state)
            << "Failed to start test for " << str;

        // repeatedly fail the motor and go through the ERR_RESET state
        for (int attempts = 0;
             attempts < kExpectedMotorAttempts - cycled_motor_starts;
             attempts++) {
            in.mi_sts = MiSts::ERR;

            auto out = g.Update(in, ++time);
            ASSERT_EQ(out.gov_sts, ERR_STARTUP_MOTOR_RESET);
            EXPECT_EQ(out.mi_cmd, motor::Command::ERR_RESET);

            // Return to Startup only once MI turns off
            ASSERT_EQ(g.Update(in, ++time).gov_sts, ERR_STARTUP_MOTOR_RESET);

            in.mi_sts = MiSts::OFF;
            out = g.Update(in, ++time);
            ASSERT_EQ(out.gov_sts, STARTUP_HV);

            // Go back to MOTOR_STARTUP state to increment counter
            in.acc_sts = AccSts::RUNNING;
            g.Update(in, ++time);
            in.di_sts = DiSts::REQUESTED_MOTOR_START;
            ASSERT_EQ(g.Update(in, ++time).gov_sts, STARTUP_MOTOR);
        }

        // the next ERR should go to MOTOR_FAULT
        in.mi_sts = MiSts::ERR;
        auto out = g.Update(in, ++time);
        ASSERT_EQ(out.gov_sts, ERR_STARTUP_MOTOR_FAULT);
        EXPECT_EQ(out.mi_cmd, motor::Command::SHUTDOWN);

        // We should NOT be able to exit this state
        in.mi_sts = MiSts::OFF;
        ASSERT_EQ(g.Update(in, ++time).gov_sts, ERR_STARTUP_MOTOR_FAULT);
    }
}

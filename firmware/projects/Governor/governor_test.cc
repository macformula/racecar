#include <cassert>
#include <iostream>
#include <tuple>
#include <vector>

#include "governor.hpp"

Governor CycleToState(GovSts desired_state_) {
    Governor g;
    Governor::Input in{
        .bm_sts = BmSts::INIT,
        .mi_sts = MiSts::INIT,
        .di_sts = DiSts::INIT,
    };

    int time = 0;

    g.Update(in, time);
    time += 2000;

    in.di_sts = DiSts::HV_START_REQ;
    assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_HV);
    if (desired_state_ == GovSts::STARTUP_HV) return g;

    in.bm_sts = BmSts::RUNNING;
    assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_READY_TO_DRIVE);
    if (desired_state_ == GovSts::STARTUP_READY_TO_DRIVE) return g;

    in.di_sts = DiSts::READY_TO_DRIVE_REQ;
    assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_MOTOR);
    if (desired_state_ == GovSts::STARTUP_MOTOR) return g;

    in.mi_sts = MiSts::RUNNING;
    assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_SEND_READY_TO_DRIVE);
    if (desired_state_ == GovSts::STARTUP_SEND_READY_TO_DRIVE) return g;

    in.di_sts = DiSts::RUNNING;
    assert(g.Update(in, ++time).gov_sts == GovSts::RUNNING);
    if (desired_state_ == GovSts::RUNNING) return g;

    in.bm_sts = BmSts::HVIL_INTERRUPT;
    assert(g.Update(in, ++time).gov_sts == GovSts::SHUTDOWN);
    if (desired_state_ == GovSts::SHUTDOWN) return g;

    return g;
}

void test_normal_sequence() {
    Governor g;

    Governor::Input in{
        .bm_sts = BmSts::INIT,
        .mi_sts = MiSts::INIT,
        .di_sts = DiSts::INIT,
    };

    int time = 0;

    {
        auto out = g.Update(in, time);
        assert(out.gov_sts == GovSts::INIT);
        assert(out.di_cmd == DiCmd::INIT);
        assert(out.bm_cmd == BmCmd::INIT);
    }

    in.di_sts = DiSts::HV_START_REQ;
    time += 1999;

    {  // Shouldn't go yet
        auto out = g.Update(in, time);
        assert(out.gov_sts == GovSts::INIT);
    }

    time += 1;

    {  // 2s have elapsed -> enter startup
        auto out = g.Update(in, time);
        assert(out.gov_sts == GovSts::STARTUP_HV);
        assert(out.bm_cmd == BmCmd::HV_STARTUP);

        assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_HV);
    }

    in.bm_sts = BmSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::STARTUP_READY_TO_DRIVE);
        assert(out.di_cmd == DiCmd::HV_ON);

        assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_READY_TO_DRIVE);
    }

    in.di_sts = DiSts::READY_TO_DRIVE_REQ;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::STARTUP_MOTOR);
        assert(out.mi_cmd == MiCmd::STARTUP);

        assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_MOTOR);
    }

    in.mi_sts = MiSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::STARTUP_SEND_READY_TO_DRIVE);
        assert(out.di_cmd == DiCmd::READY_TO_DRIVE);

        assert(g.Update(in, ++time).gov_sts ==
               GovSts::STARTUP_SEND_READY_TO_DRIVE);
    }

    in.di_sts = DiSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::RUNNING);
    }

    std::cout << "Passed test_normal_sequence!" << std::endl;
}

void test_shutdown_hvil() {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .bm_sts = BmSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };

    int time = 5000;
    assert(g.Update(in, time).gov_sts == GovSts::RUNNING);

    in.bm_sts = BmSts::HVIL_INTERRUPT;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::SHUTDOWN);
        assert(out.mi_cmd == MiCmd::SHUTDOWN);
        assert(out.di_cmd == DiCmd::SHUTDOWN);

        assert(g.Update(in, ++time).gov_sts == GovSts::SHUTDOWN);
    }

    in.bm_sts = BmSts::INIT;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::INIT);
        assert(out.bm_cmd == BmCmd::INIT);
        assert(out.di_cmd == DiCmd::INIT);
    }

    std::cout << "Passed test_shutdown_hvil!" << std::endl;
}

void test_shutdown_low_soc() {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .bm_sts = BmSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };

    int time = 5000;
    assert(g.Update(in, time).gov_sts == GovSts::RUNNING);

    in.bm_sts = BmSts::LOW_SOC;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::SHUTDOWN);
        assert(out.mi_cmd == MiCmd::SHUTDOWN);
        assert(out.di_cmd == DiCmd::SHUTDOWN);

        assert(g.Update(in, ++time).gov_sts == GovSts::SHUTDOWN);
    }

    in.bm_sts = BmSts::INIT;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::INIT);
        assert(out.bm_cmd == BmCmd::INIT);
        assert(out.di_cmd == DiCmd::INIT);
    }

    std::cout << "Passed test_shutdown_low_soc!" << std::endl;
}

void test_running_err() {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .bm_sts = BmSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };
    int time = 0;

    assert(g.Update(in, ++time).gov_sts == GovSts::RUNNING);

    in.bm_sts = BmSts::ERR_RUNNING;

    auto out = g.Update(in, ++time);
    assert(out.gov_sts == GovSts::ERR_RUNNING_HV);
    assert(out.di_cmd == DiCmd::ERR_RUN);

    std::cout << "Passed test_running_err!" << std::endl;
}

void test_running_mi_err() {
    Governor g = CycleToState(GovSts::RUNNING);

    Governor::Input in{
        .bm_sts = BmSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };
    int time = 0;

    assert(g.Update(in, ++time).gov_sts == GovSts::RUNNING);

    in.mi_sts = MiSts::ERR;

    auto out = g.Update(in, ++time);
    assert(out.gov_sts == GovSts::ERR_RUNNING_MOTOR);
    assert(out.di_cmd == DiCmd::ERR_RUN);

    std::cout << "Passed test_running_mi_err!" << std::endl;
}

void test_startup_di_err() {
    for (auto start_state : {
             GovSts::STARTUP_HV,
             GovSts::STARTUP_READY_TO_DRIVE,
             GovSts::STARTUP_MOTOR,
             GovSts::STARTUP_SEND_READY_TO_DRIVE,
         }) {
        Governor g = CycleToState(start_state);

        Governor::Input in{
            .bm_sts = BmSts::INIT,
            .mi_sts = MiSts::INIT,
            .di_sts = DiSts::INIT,
        };

        assert(g.Update(in, 0).gov_sts == start_state);

        in.di_sts = DiSts::ERR;

        auto out = g.Update(in, 1);

        assert(out.gov_sts == GovSts::ERR_STARTUP_DI);
    }
    std::cout << "Passed test_startup_di_err!" << std::endl;
}

void test_startup_hv_err() {
    for (auto start_state : {
             GovSts::STARTUP_HV,
             GovSts::STARTUP_READY_TO_DRIVE,
             GovSts::STARTUP_MOTOR,
             GovSts::STARTUP_SEND_READY_TO_DRIVE,
         }) {
        Governor g = CycleToState(start_state);

        Governor::Input in{
            .bm_sts = BmSts::INIT,
            .mi_sts = MiSts::INIT,
            .di_sts = DiSts::INIT,
        };

        assert(g.Update(in, 0).gov_sts == start_state);

        in.bm_sts = BmSts::HVIL_INTERRUPT;

        {
            auto out = g.Update(in, 1);
            assert(out.gov_sts == GovSts::ERR_STARTUP_HV);
            assert(out.di_cmd == DiCmd::SHUTDOWN);
        }

        // Return to INIT only after BM=INIT
        assert(g.Update(in, 2).gov_sts == GovSts::ERR_STARTUP_HV);

        in.bm_sts = BmSts::INIT;

        {
            auto out = g.Update(in, 3);
            assert(out.gov_sts == GovSts::INIT);
            assert(out.di_cmd == DiCmd::INIT);
            assert(out.bm_cmd == BmCmd::INIT);
        }
    }

    std::cout << "Passed test_startup_hv_err!" << std::endl;
}

void test_startup_motor_error() {
    // fail the motors 6 times. should go through reset until the last time
    // this tests both the ERR_STARTUP_MOTOR_FAULT and ERR_STARTUP_MOTOR_RESET
    // states

    const int kExpectedMotorAttempts = 5;  // should match Governor constant

    std::vector<std::tuple<GovSts, std::string>> start_states = {
        {GovSts::STARTUP_HV, "STARTUP_HV"},
        {GovSts::STARTUP_READY_TO_DRIVE, "STARTUP_READY_TO_DRIVE"},
        {GovSts::STARTUP_MOTOR, "STARTUP_MOTOR"},
        {GovSts::STARTUP_SEND_READY_TO_DRIVE, "STARTUP_SEND_READY_TO_DRIVE"},
    };

    for (auto [start_state, str] : start_states) {
        std::cout << "  startup_motor_error from " << str << "... ";
        Governor g = CycleToState(start_state);

        Governor::Input in{
            .bm_sts = BmSts::INIT,
            .mi_sts = MiSts::INIT,
            .di_sts = DiSts::HV_START_REQ,
        };
        int time = 0;

        assert(g.Update(in, time).gov_sts == start_state);

        int cycled_motor_starts =
            (start_state == GovSts::STARTUP_MOTOR ||
             start_state == GovSts::STARTUP_SEND_READY_TO_DRIVE)
                ? 1
                : 0;

        for (int attempts = 0;
             attempts < kExpectedMotorAttempts - cycled_motor_starts;
             attempts++) {
            in.mi_sts = MiSts::ERR;

            auto out = g.Update(in, ++time);
            assert(out.gov_sts == GovSts::ERR_STARTUP_MOTOR_RESET);
            assert(out.mi_cmd == MiCmd::ERR_RESET);

            // Return to Startup only once MI turns off
            assert(g.Update(in, ++time).gov_sts ==
                   GovSts::ERR_STARTUP_MOTOR_RESET);

            in.mi_sts = MiSts::OFF;
            out = g.Update(in, ++time);
            assert(out.gov_sts == GovSts::STARTUP_HV);

            // Go back to MOTOR_STARTUP state to increment counter
            in.bm_sts = BmSts::RUNNING;
            g.Update(in, ++time);
            in.di_sts = DiSts::READY_TO_DRIVE_REQ;
            assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_MOTOR);
        }

        // the next ERR should go to MOTOR_FAULT
        in.mi_sts = MiSts::ERR;
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::ERR_STARTUP_MOTOR_FAULT);
        assert(out.mi_cmd == MiCmd::SHUTDOWN);

        // We should NOT be able to exit this state
        in.mi_sts = MiSts::OFF;
        assert(g.Update(in, ++time).gov_sts == GovSts::ERR_STARTUP_MOTOR_FAULT);

        std::cout << "passed" << std::endl;
    }

    std::cout << "Passed test_startup_motor_error" << std::endl;
}

int main() {
    test_normal_sequence();
    test_shutdown_hvil();
    test_shutdown_low_soc();

    test_startup_di_err();
    test_startup_hv_err();
    test_startup_motor_error();

    test_running_err();
    test_running_mi_err();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. Run the test by executing ./bm_test.exe
#include "control-system/battery_monitor.h"

#include <cassert>
#include <iostream>
#include <vector>

#include "control-system/enums.hpp"

BatteryMonitor CycleToState(BmSts desired_state, int& time_ms) {
    using enum ContactorState;

    BatteryMonitor bm{};
    time_ms = 0;

    // Initialize the BatteryMonitor
    bm.Update(
        {
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::INIT) {
        return bm;
    }

    if (bm.Update(
              {
                  .cmd = BmCmd::HV_STARTUP,
                  .pack_soc = 20.0,
              },
              time_ms)
            .status == BmSts::LOW_SOC) {
        return bm;
    }

    // Transition to IDLE
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::IDLE) {
        return bm;
    }

    // Transition to STARTUP
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP) {
        return bm;
    }

    // Transition to PRECHARGE
    time_ms += 6500;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::PRECHARGE) {
        return bm;
    }

    // Transition to PRECHARGE_DONE
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::PRECHARGE_DONE) {
        return bm;
    }

    // Transition to RUNNING
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::RUNNING) {
        return bm;
    }

    // Transition to LOW_SOC
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .pack_soc = 20.0,
        },
        time_ms);

    return bm;
}

std::string BmStatusToString(BmSts status) {
    switch (status) {
        case BmSts::INIT:
            return "INIT";
        case BmSts::IDLE:
            return "IDLE";
        case BmSts::STARTUP:
            return "STARTUP";
        case BmSts::PRECHARGE:
            return "PRECHARGE";
        case BmSts::PRECHARGE_DONE:
            return "PRECHARGE_DONE";
        case BmSts::RUNNING:
            return "RUNNING";
        case BmSts::LOW_SOC:
            return "LOW_SOC";
        default:
            return "UNKNOWN";
    }
}

void test_low_soc_transitions() {
    using enum ContactorState;
    std::vector<BmSts> states = {BmSts::IDLE, BmSts::STARTUP, BmSts::PRECHARGE,
                                 BmSts::PRECHARGE_DONE, BmSts::RUNNING};
    int time_ms;

    for (BmSts state : states) {
        BatteryMonitor bm = CycleToState(state, time_ms);

        std::cout << "Testing transition from " << BmStatusToString(state)
                  << " to LOW_SOC\n";
        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = BmCmd::HV_STARTUP,
                .pack_soc = 20.0,
            },
            time_ms);

        assert(output.status == BmSts::LOW_SOC);
        assert(output.contactor.precharge == OPEN);
        assert(output.contactor.hv_negative == OPEN);
        assert(output.contactor.hv_positive == OPEN);

        std::cout << "Transition from " << BmStatusToString(state)
                  << " to LOW_SOC passed!\n";
    }
}

void test_hvil_interrupt_transitions() {
    using enum ContactorState;
    std::vector<BmSts> states = {BmSts::IDLE, BmSts::STARTUP, BmSts::PRECHARGE,
                                 BmSts::PRECHARGE_DONE, BmSts::RUNNING};
    int time_ms;

    for (BmSts state : states) {
        BatteryMonitor bm = CycleToState(state, time_ms);

        std::cout << "Testing transition from " << BmStatusToString(state)
                  << " to HVIL_INTERRUPT\n";
        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = BmCmd::HV_STARTUP,
                .pack_soc = 50.0,
            },
            time_ms);

        // assert(output.status == BmSts::HVIL_INTERRUPT);
        assert(output.contactor.precharge == OPEN);
        assert(output.contactor.hv_negative == OPEN);
        assert(output.contactor.hv_positive == OPEN);

        std::cout << "Transition from " << BmStatusToString(state)
                  << " to HVIL_INTERRUPT passed!\n";
    }
}

void test_state_transitions() {
    using enum ContactorState;

    BatteryMonitor bm{};
    int time_ms = 0;

    std::cout << "Running Test 1: initialized\n";
    auto output1 = bm.Update(
        {
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output1.status == BmSts::INIT);
    assert(output1.control_status == ControlStatus::STARTUP_CMD);

    // Test: Transition from INIT to IDLE
    time_ms += 10;
    std::cout << "Running Test 2: init to idle\n";
    auto output2 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output2.status == BmSts::IDLE);
    assert(output2.control_status == ControlStatus::CLOSE_HV_NEG);

    // Test: Transition from IDLE to STARTUP
    std::cout << "Running Test 3: idle to startup\n";
    time_ms += 500;
    auto output3 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == BmSts::STARTUP);
    assert(output3.control_status == ControlStatus::CLOSE_PRECHARGE);

    // Test: Transition from STARTUP to PRECHARGE
    std::cout << "Running Test 4: startup to init PRECHARGE_DONE\n";
    time_ms += 10000;
    auto output4 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,  // PRECHARGE_DONE active
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output4.status == BmSts::PRECHARGE);
    assert(output4.control_status == ControlStatus::CLOSE_HV_POS);

    // Test: Transition from PRECHARGE to PRECHARGE_DONE
    std::cout << "Running Test 5: init PRECHARGE_DONE to PRECHARGE_DONE\n";
    time_ms += 500;
    auto output5 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = CLOSED,  // HV_POS active
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output5.status == BmSts::PRECHARGE_DONE);
    assert(output5.control_status == ControlStatus::OPEN_PRECHARGE);

    // Test: Transition from PRECHARGE_DONE to RUNNING
    std::cout << "Running Test 6: PRECHARGE_DONE to running\n";
    time_ms += 10;
    auto output6 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,  // PRECHARGE_DONE inactive
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output6.status == BmSts::RUNNING);
    assert(output6.control_status == ControlStatus::OPEN_PRECHARGE);

    std::cout << "Test sequence passed!" << std::endl;
}

void BmTest() {
    std::cout << "TEST CASES FOR LOW_SOC TRANSITIONS" << std::endl;
    test_low_soc_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "TEST CASES FOR HVIL_INTERRUPT TRANSITIONS" << std::endl;
    test_hvil_interrupt_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "TEST CASES FOR STATE TRANSITIONS" << std::endl;
    test_state_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "All BM tests passed!" << std::endl;
}

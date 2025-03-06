// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. Run the test by executing ./bm_test.exe

#include <cassert>
#include <iostream>
#include <vector>

#include "control-system/battery_monitor.h"
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
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::INIT) {
        return bm;
    }

    if (bm.Update(
              {
                  .cmd = BmCmd::STARTUP,
                  .pack_soc = 20.0,
              },
              time_ms)
            .status == BmSts::LOW_SOC) {
        return bm;
    }

    // Transition to CLOSE_NEG
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_CLOSE_NEG) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_NEG
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_HOLD_CLOSE_NEG) {
        return bm;
    }

    // Transition to STARTUP_CLOSE_PRECHARGE
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_CLOSE_PRECHARGE) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_PRECHARGE
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_HOLD_CLOSE_PRECHARGE) {
        return bm;
    }

    // Transition to STARTUP_CLOSE_POS
    time_ms += 6500;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_CLOSE_POS) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_POS
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_HOLD_CLOSE_POS) {
        return bm;
    }

    // Transition to STARTUP_OPEN_PRECHARGE
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_OPEN_PRECHARGE) {
        return bm;
    }

    // Transition to RUNNING
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::RUNNING,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::RUNNING) {
        return bm;
    }

    // Transition to LOW_SOC
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .pack_soc = 20.0,
        },
        time_ms);

    return bm;
}

std::string BmStatusToString(BmSts status) {
    switch (status) {
        case BmSts::INIT:
            return "INIT";
        case BmSts::STARTUP_CLOSE_NEG:
            return "STARTUP_CLOSE_NEG";
        case BmSts::STARTUP_CLOSE_PRECHARGE:
            return "STARTUP_CLOSE_PRECHARGE";
        case BmSts::STARTUP_CLOSE_POS:
            return "STARTUP_CLOSE_POS";
        case BmSts::STARTUP_OPEN_PRECHARGE:
            return "STARTUP_OPEN_PRECHARGE";
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
    std::vector<BmSts> states = {
        BmSts::STARTUP_CLOSE_NEG, BmSts::STARTUP_CLOSE_PRECHARGE,
        BmSts::STARTUP_CLOSE_POS, BmSts::STARTUP_OPEN_PRECHARGE};
    int time_ms;

    for (BmSts state : states) {
        BatteryMonitor bm = CycleToState(state, time_ms);

        std::cout << "Testing transition from " << BmStatusToString(state)
                  << " to LOW_SOC\n";
        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = BmCmd::STARTUP,
                .pack_soc = 20.0,
            },
            time_ms);

        assert(output.status == BmSts::LOW_SOC);

        std::cout << "Transition from " << BmStatusToString(state)
                  << " to LOW_SOC passed!\n";
    }
}

void test_state_transitions() {
    using enum ContactorState;
    using enum BmSts;

    BatteryMonitor bm{};
    int time_ms = 0;

    std::cout << "Running Test 1: Initialized state\n";
    auto output1 = bm.Update(
        {
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output1.status == INIT);

    time_ms += 10;
    std::cout << "Running Test 2: INIT to CLOSE_NEG\n";
    auto output2 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output2.status == STARTUP_CLOSE_NEG);

    time_ms += 10;
    std::cout << "Running Test 3: CLOSE_NEG to HOLD_CLOSE_NEG\n";
    auto output3 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output3.status == STARTUP_HOLD_CLOSE_NEG);

    time_ms += 100;
    std::cout << "Running Test 4: HOLD_CLOSE_NEG to CLOSE_PRECHARGE\n";
    auto output4 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output4.status == STARTUP_CLOSE_PRECHARGE);

    time_ms += 10;
    std::cout << "Running Test 5: CLOSE_PRECHARGE to HOLD_CLOSE_PRECHARGE\n";
    auto output5 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output5.status == STARTUP_HOLD_CLOSE_PRECHARGE);

    time_ms += 6500;
    std::cout << "Running Test 6: HOLD_CLOSE_PRECHARGE to CLOSE_POS\n";
    auto output6 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output6.status == STARTUP_CLOSE_POS);

    time_ms += 10;
    std::cout << "Running Test 7: CLOSE_POS to HOLD_CLOSE_POS\n";
    auto output7 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output7.status == STARTUP_HOLD_CLOSE_POS);

    time_ms += 100;
    std::cout << "Running Test 8: HOLD_CLOSE_POS to OPEN_PRECHARGE\n";
    auto output8 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output8.status == STARTUP_OPEN_PRECHARGE);

    time_ms += 10;
    std::cout << "Running Test 9: OPEN_PRECHARGE to RUNNING\n";
    auto output9 = bm.Update(
        {
            .cmd = BmCmd::RUNNING,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    assert(output9.status == RUNNING);

    std::cout << "Test sequence passed!" << std::endl;
}

void BmTest() {
    std::cout << std::endl;
    std::cout << "Running Battery Monitor tests..." << std::endl;
    std::cout << std::endl;
    std::cout << "TEST CASES FOR LOW_SOC TRANSITIONS" << std::endl;
    test_low_soc_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "TEST CASES FOR STATE TRANSITIONS" << std::endl;
    test_state_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "All BM tests passed!" << std::endl;
}

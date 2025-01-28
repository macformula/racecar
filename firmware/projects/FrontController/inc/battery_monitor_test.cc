// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. Run the test by executing ./bm_test.exe
#include "battery_monitor.h"

#include <cassert>
#include <iostream>
#include <vector>

BatteryMonitor CycleToState(BmStatus desired_state, int& time_ms) {
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
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmStatus::INIT) {
        return bm;
    }

    // Check for HVIL_INTERRUPT or LOW_SOC transition
    if (bm.Update(
              {
                  .cmd = BmCmd::HV_STARTUP,
                  .hvil_status = OPEN,
                  .pack_soc = 50.0,
              },
              time_ms)
            .status == BmStatus::HVIL_INTERRUPT) {
        return bm;
    }

    if (bm.Update(
              {
                  .cmd = BmCmd::HV_STARTUP,
                  .hvil_status = CLOSED,
                  .pack_soc = 20.0,
              },
              time_ms)
            .status == BmStatus::LOW_SOC) {
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
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmStatus::IDLE) {
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
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmStatus::STARTUP) {
        return bm;
    }

    // Transition to INIT_PRECHARGE
    time_ms += 6500;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmStatus::INIT_PRECHARGE) {
        return bm;
    }

    // Transition to PRECHARGE
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmStatus::PRECHARGE) {
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
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmStatus::RUNNING) {
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

std::string BmStatusToString(BmStatus status) {
    switch (status) {
        case BmStatus::INIT:
            return "INIT";
        case BmStatus::IDLE:
            return "IDLE";
        case BmStatus::STARTUP:
            return "STARTUP";
        case BmStatus::INIT_PRECHARGE:
            return "INIT_PRECHARGE";
        case BmStatus::PRECHARGE:
            return "PRECHARGE";
        case BmStatus::RUNNING:
            return "RUNNING";
        case BmStatus::LOW_SOC:
            return "LOW_SOC";
        case BmStatus::HVIL_INTERRUPT:
            return "HVIL_INTERRUPT";
        default:
            return "UNKNOWN";
    }
}

void test_low_soc_transitions() {
    using enum ContactorState;
    std::vector<BmStatus> states = {BmStatus::IDLE, BmStatus::STARTUP,
                                    BmStatus::INIT_PRECHARGE,
                                    BmStatus::PRECHARGE, BmStatus::RUNNING};
    int time_ms;

    for (BmStatus state : states) {
        BatteryMonitor bm = CycleToState(state, time_ms);

        std::cout << "Testing transition from " << BmStatusToString(state)
                  << " to LOW_SOC\n";
        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = BmCmd::HV_STARTUP,
                .hvil_status = CLOSED,
                .pack_soc = 20.0,
            },
            time_ms);

        assert(output.status == BmStatus::LOW_SOC);
        assert(output.contactor.precharge == OPEN);
        assert(output.contactor.hv_negative == OPEN);
        assert(output.contactor.hv_positive == OPEN);

        std::cout << "Transition from " << BmStatusToString(state)
                  << " to LOW_SOC passed!\n";
    }
}

void test_hvil_interrupt_transitions() {
    using enum ContactorState;
    std::vector<BmStatus> states = {BmStatus::IDLE, BmStatus::STARTUP,
                                    BmStatus::INIT_PRECHARGE,
                                    BmStatus::PRECHARGE, BmStatus::RUNNING};
    int time_ms;

    for (BmStatus state : states) {
        BatteryMonitor bm = CycleToState(state, time_ms);

        std::cout << "Testing transition from " << BmStatusToString(state)
                  << " to HVIL_INTERRUPT\n";
        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = BmCmd::HV_STARTUP,
                .hvil_status = OPEN,
                .pack_soc = 50.0,
            },
            time_ms);

        assert(output.status == BmStatus::HVIL_INTERRUPT);
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
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output1.status == BmStatus::INIT);
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
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output2.status == BmStatus::IDLE);
    assert(output2.control_status == ControlStatus::CLOSE_HV_NEG);

    // Test: Transition from IDLE to STARTUP
    std::cout << "Running Test 3: idle to startup\n";
    time_ms += 100;
    auto output3 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == BmStatus::STARTUP);
    assert(output3.control_status == ControlStatus::CLOSE_PRECHARGE);

    // Test: Transition from STARTUP to INIT_PRECHARGE
    std::cout << "Running Test 4: startup to init precharge\n";
    time_ms += 6500;
    auto output4 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,  // Precharge active
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output4.status == BmStatus::INIT_PRECHARGE);
    assert(output4.control_status == ControlStatus::CLOSE_HV_POS);

    // Test: Transition from INIT_PRECHARGE to PRECHARGE
    std::cout << "Running Test 5: init precharge to precharge\n";
    time_ms += 100;
    auto output5 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = CLOSED,  // HV_POS active
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output5.status == BmStatus::PRECHARGE);
    assert(output5.control_status == ControlStatus::OPEN_PRECHARGE);

    // Test: Transition from PRECHARGE to RUNNING
    std::cout << "Running Test 6: precharge to running\n";
    time_ms += 10;
    auto output6 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,  // Precharge inactive
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output6.status == BmStatus::RUNNING);
    assert(output6.control_status == ControlStatus::OPEN_PRECHARGE);

    std::cout << "Test sequence passed!" << std::endl;
}

int main() {
    std::cout << "TEST CASES FOR LOW_SOC TRANSITIONS" << std::endl;
    test_low_soc_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "TEST CASES FOR HVIL_INTERRUPT TRANSITIONS" << std::endl;
    test_hvil_interrupt_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "TEST CASES FOR STATE TRANSITIONS" << std::endl;
    test_state_transitions();
    std::cout << std::endl << std::endl;

    std::cout << "All tests passed!" << std::endl;

    return 0;
}

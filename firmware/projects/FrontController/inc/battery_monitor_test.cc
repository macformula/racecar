// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. Run the test by executing ./bm_test.exe

#include "battery_monitor.h"

#include <cassert>
#include <iostream>

void test_sequence1() {
    using enum ContactorState;

    // the tests should show that your code matches the Simulink model's
    // expected behavior
    BatteryMonitor bm{};
    int time_ms = 0;

    // Test: Initialization
    std::cout << "Running Test 1:\n";
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
    assert(output1.contactor.precharge == OPEN);
    assert(output1.contactor.hv_negative == OPEN);
    assert(output1.contactor.hv_positive == OPEN);

    // Test: Transition from INIT to IDLE
    time_ms += 10;  // next update comes 10 ms later
    std::cout << "Running Test 2:\n";
    auto output2 = bm.Update(
        // this input matches the condition to move from InitialState to
        // StartupState1 in Simulink
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
    assert(output2.contactor.precharge == OPEN);
    assert(output2.contactor.hv_negative == CLOSED);
    assert(output2.contactor.hv_positive == OPEN);

    // Test: Transition from IDLE to STARTUP
    std::cout << "Running Test 3:\n";
    time_ms += 100;
    auto output3 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,  // HV_NEG contactor active
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == BmStatus::STARTUP);
    assert(output3.contactor.precharge == CLOSED);
    assert(output3.contactor.hv_negative == CLOSED);
    assert(output3.contactor.hv_positive == OPEN);

    // Test: Transition from STARTUP to INIT_PRECHARGE
    std::cout << "Running Test 4:\n";
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
    assert(output4.contactor.precharge == CLOSED);
    assert(output4.contactor.hv_negative == CLOSED);
    assert(output4.contactor.hv_positive == CLOSED);

    // Test: Transition from INIT_PRECHARGE to PRECHARGE
    std::cout << "Running Test 5:\n";
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
    assert(output5.contactor.precharge == OPEN);
    assert(output5.contactor.hv_negative == CLOSED);
    assert(output5.contactor.hv_positive == CLOSED);

    // Test: Transition from PRECHARGE to RUNNING
    std::cout << "Running Test 6:\n";
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
    assert(output6.contactor.precharge == OPEN);
    assert(output6.contactor.hv_negative == CLOSED);
    assert(output6.contactor.hv_positive == CLOSED);

    // Test: Transition to LOW_SOC
    std::cout << "Running Test 7:\n";
    time_ms += 10;
    auto output7 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 20.0,  // SOC below LOW_SOC threshold
        },
        time_ms);

    assert(output7.status == BmStatus::LOW_SOC);
    assert(output7.contactor.precharge == OPEN);
    assert(output7.contactor.hv_negative == OPEN);
    assert(output7.contactor.hv_positive == OPEN);

    std::cout << "Test sequence 1 passed!" << std::endl;
}

void test_sequence2() {
    using enum ContactorState;

    // the tests should show that your code matches the Simulink model's
    // expected behavior
    BatteryMonitor bm{};
    int time_ms = 0;

    // Test: Initialization
    std::cout << "Running Test 1:\n";
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
    assert(output1.contactor.precharge == OPEN);
    assert(output1.contactor.hv_negative == OPEN);
    assert(output1.contactor.hv_positive == OPEN);

    // Test: Transition from INIT to IDLE
    time_ms += 10;  // next update comes 10 ms later
    std::cout << "Running Test 2:\n";
    auto output2 = bm.Update(
        // this input matches the condition to move from InitialState to
        // StartupState1 in Simulink
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
    assert(output2.contactor.precharge == OPEN);
    assert(output2.contactor.hv_negative == CLOSED);
    assert(output2.contactor.hv_positive == OPEN);

    // Test: Transition from IDLE to STARTUP
    std::cout << "Running Test 3:\n";
    time_ms += 100;
    auto output3 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,  // HV_NEG contactor active
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == BmStatus::STARTUP);
    assert(output3.contactor.precharge == CLOSED);
    assert(output3.contactor.hv_negative == CLOSED);
    assert(output3.contactor.hv_positive == OPEN);

    // Test: Transition from STARTUP to INIT_PRECHARGE
    std::cout << "Running Test 4:\n";
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
    assert(output4.contactor.precharge == CLOSED);
    assert(output4.contactor.hv_negative == CLOSED);
    assert(output4.contactor.hv_positive == CLOSED);

    // Test: Transition from INIT_PRECHARGE to PRECHARGE
    std::cout << "Running Test 5:\n";
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
    assert(output5.contactor.precharge == OPEN);
    assert(output5.contactor.hv_negative == CLOSED);
    assert(output5.contactor.hv_positive == CLOSED);

    // Test: Transition from PRECHARGE to RUNNING
    std::cout << "Running Test 6:\n";
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
    assert(output6.contactor.precharge == OPEN);
    assert(output6.contactor.hv_negative == CLOSED);
    assert(output6.contactor.hv_positive == CLOSED);

    // Test: HVIL_INTERRUPT and back to INIT
    std::cout << "Running Test 7:\n";
    time_ms += 10;
    auto output8 = bm.Update(
        {
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = OPEN,  // HVIL disabled
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output8.status == BmStatus::HVIL_INTERRUPT);
    assert(output8.contactor.precharge == OPEN);
    assert(output8.contactor.hv_negative == OPEN);
    assert(output8.contactor.hv_positive == OPEN);

    // Simulate HVIL restored after 5000ms
    std::cout << "Running Test 8:\n";
    time_ms += 5000;
    auto output9 = bm.Update(
        {
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .hvil_status = CLOSED,  // HVIL restored
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output9.status == BmStatus::INIT);
    assert(output9.contactor.precharge == OPEN);
    assert(output9.contactor.hv_positive == OPEN);
    assert(output9.contactor.hv_negative == OPEN);

    std::cout << "Test sequence 2 passed!" << std::endl;
}

int main() {
    test_sequence1();
    std::cout << std::endl << std::endl;
    test_sequence2();
    std::cout << "All tests passed!" << std::endl;

    return 0;
}

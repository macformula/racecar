// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. Run the test by executing ./bm_test.exe

#include "battery_monitor.h"

#include <cassert>
#include <iostream>

void test_sequence1() {
    // the tests should show that your code matches the Simulink model's
    // expected behavior
    BatteryMonitor bm{};
    int time_ms = 0;

    // Test: Initialization
    std::cout << "Running Test 1:\n";
    BmOutput output1 = bm.update(
        BmInput{
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .hvil_status = true,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output1.status == BmStatus::INIT);
    assert(output1.contactor.precharge_contactor_cmd == OPEN);
    assert(output1.contactor.hv_neg_contactor_cmd == OPEN);
    assert(output1.contactor.hv_pos_contactor_cmd == OPEN);

    // Test: Transition from INIT to IDLE
    time_ms += 10;  // next update comes 10 ms later
    std::cout << "Running Test 2:\n";
    BmOutput output2 = bm.update(
        // this input matches the condition to move from InitialState to
        // StartupState1 in Simulink
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output2.status == BmStatus::IDLE);
    assert(output2.contactor.precharge_contactor_cmd == OPEN);
    assert(output2.contactor.hv_neg_contactor_cmd == CLOSED);
    assert(output2.contactor.hv_pos_contactor_cmd == OPEN);

    // Test: Transition from IDLE to STARTUP
    std::cout << "Running Test 3:\n";
    time_ms += 100;
    BmOutput output3 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,  // HV_NEG contactor active
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == BmStatus::STARTUP);
    assert(output3.contactor.precharge_contactor_cmd == CLOSED);
    assert(output3.contactor.hv_neg_contactor_cmd == CLOSED);
    assert(output3.contactor.hv_pos_contactor_cmd == OPEN);

    // Test: Transition from STARTUP to INIT_PRECHARGE
    std::cout << "Running Test 4:\n";
    time_ms += 6500;
    BmOutput output4 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,  // Precharge active
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output4.status == BmStatus::INIT_PRECHARGE);
    assert(output4.contactor.precharge_contactor_cmd == CLOSED);
    assert(output4.contactor.hv_neg_contactor_cmd == CLOSED);
    assert(output4.contactor.hv_pos_contactor_cmd == CLOSED);

    // Test: Transition from INIT_PRECHARGE to PRECHARGE
    std::cout << "Running Test 5:\n";
    time_ms += 100;
    BmOutput output5 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = CLOSED,
            .hv_pos_contactor_states = CLOSED,  // HV_POS active
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output5.status == BmStatus::PRECHARGE);
    assert(output5.contactor.precharge_contactor_cmd == OPEN);
    assert(output5.contactor.hv_neg_contactor_cmd == CLOSED);
    assert(output5.contactor.hv_pos_contactor_cmd == CLOSED);

    // Test: Transition from PRECHARGE to RUNNING
    std::cout << "Running Test 6:\n";
    time_ms += 10;
    BmOutput output6 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,  // Precharge inactive
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output6.status == BmStatus::RUNNING);
    assert(output6.contactor.precharge_contactor_cmd == OPEN);
    assert(output6.contactor.hv_neg_contactor_cmd == CLOSED);
    assert(output6.contactor.hv_pos_contactor_cmd == CLOSED);

    // Test: Transition to LOW_SOC
    std::cout << "Running Test 7:\n";
    time_ms += 10;
    BmOutput output7 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = CLOSED,
            .pack_soc = 20.0,  // SOC below LOW_SOC threshold
        },
        time_ms);

    assert(output7.status == BmStatus::LOW_SOC);
    assert(output7.contactor.precharge_contactor_cmd == OPEN);
    assert(output7.contactor.hv_pos_contactor_cmd == OPEN);
    assert(output7.contactor.hv_neg_contactor_cmd == OPEN);

    // Test: HVIL_INTERRUPT and back to INIT
    std::cout << "Running Test 8:\n";
    time_ms += 10;
    BmOutput output8 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = CLOSED,
            .hv_neg_contactor_states = CLOSED,
            .hvil_status = OPEN,  // HVIL disabled
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output8.status == BmStatus::HVIL_INTERRUPT);
    assert(output8.contactor.precharge_contactor_cmd == OPEN);
    assert(output8.contactor.hv_pos_contactor_cmd == OPEN);
    assert(output8.contactor.hv_neg_contactor_cmd == OPEN);

    // Simulate HVIL restored after 5000ms
    std::cout << "Running Test 9:\n";
    time_ms += 5000;
    BmOutput output9 = bm.update(
        BmInput{
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .hv_pos_contactor_states = OPEN,
            .hv_neg_contactor_states = OPEN,
            .hvil_status = CLOSED,  // HVIL restored
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output9.status == BmStatus::INIT);
    assert(output9.contactor.precharge_contactor_cmd == OPEN);
    assert(output9.contactor.hv_pos_contactor_cmd == OPEN);
    assert(output9.contactor.hv_neg_contactor_cmd == OPEN);

    std::cout << "Test sequence 1 passed!" << std::endl;
}

int main() {
    test_sequence1();
    std::cout << "All tests passed!" << std::endl;

    return 0;
}

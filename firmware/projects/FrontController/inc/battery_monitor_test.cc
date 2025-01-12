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
    BmOutput output1 = bm.update(
        BmInput{
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = false,
            .hv_pos_contactor_states = false,
            .hv_neg_contactor_states = false,
            .hvil_status = false,
            .pack_soc = 0.0,
        },
        time_ms);

    assert(output1.status == BmStatus::INIT);

    time_ms += 10;  // next update comes 10 ms later

    // Test: Transition from INIT to IDLE
    BmOutput output2 = bm.update(
        // this input matches the condition to move from InitialState to
        // StartupState1 in Simulink
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = false,
            .hv_pos_contactor_states = false,
            .hv_neg_contactor_states = false,
            .hvil_status = true,
            .pack_soc = 0.0,
        },
        time_ms);

    assert(output2.status == BmStatus::IDLE);

    // extend the test sequence to cover more states and transitions

    // Test: Transition from IDLE to STARTUP
    time_ms += 10;
    BmOutput output3 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = false,
            .hv_pos_contactor_states = false,
            .hv_neg_contactor_states = true,  // HV_NEG contactor active
            .hvil_status = true,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == BmStatus::STARTUP);

    // Test: Transition from STARTUP to INIT_PRECHARGE
    time_ms += 10;
    BmOutput output4 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = true,  // Precharge active
            .hv_pos_contactor_states = false,
            .hv_neg_contactor_states = true,
            .hvil_status = true,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output4.status == BmStatus::INIT_PRECHARGE);

    // Test: Transition from INIT_PRECHARGE to PRECHARGE
    time_ms += 10;
    BmOutput output5 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = true,
            .hv_pos_contactor_states = true,  // HV_POS active
            .hv_neg_contactor_states = true,
            .hvil_status = true,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output5.status == BmStatus::PRECHARGE);

    // Test: Transition from PRECHARGE to RUNNING
    time_ms += 10;
    BmOutput output6 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = false,  // Precharge inactive
            .hv_pos_contactor_states = true,
            .hv_neg_contactor_states = true,
            .hvil_status = true,
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output6.status == BmStatus::RUNNING);

    // Test: Transition to LOW_SOC
    time_ms += 10;
    BmOutput output7 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = false,
            .hv_pos_contactor_states = true,
            .hv_neg_contactor_states = true,
            .hvil_status = true,
            .pack_soc = 20.0,  // SOC below LOW_SOC threshold
        },
        time_ms);

    assert(output7.status == BmStatus::LOW_SOC);

    // Test: HVIL_INTERRUPT and back to INIT
    time_ms += 10;
    BmOutput output8 = bm.update(
        BmInput{
            .cmd = BmCmd::HV_STARTUP,
            .precharge_contactor_states = false,
            .hv_pos_contactor_states = true,
            .hv_neg_contactor_states = true,
            .hvil_status = false,  // HVIL disabled
            .pack_soc = 20.0,
        },
        time_ms);

    assert(output8.status == BmStatus::HVIL_INTERRUPT);

    // Simulate HVIL restored after 5000ms
    time_ms += 5000;
    BmOutput output9 = bm.update(
        BmInput{
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = false,
            .hv_pos_contactor_states = false,
            .hv_neg_contactor_states = false,
            .hvil_status = true,  // HVIL restored
            .pack_soc = 20.0,
        },
        time_ms);

    assert(output9.status == BmStatus::INIT);

    std::cout << "Test sequence 1 passed!" << std::endl;
}

int main() {
    test_sequence1();
    std::cout << "All tests passed!" << std::endl;

    return 0;
}

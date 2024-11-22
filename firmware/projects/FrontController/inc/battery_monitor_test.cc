// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. run the test by running ./bm_test.exe

#include "battery_monitor.h"

#include <cassert>
#include <iostream>

void test_sequence1() {
    // the tests should show that your code matches the simulink model's
    // expected behavior
    BatteryMonitor bm{};
    int time_ms = 0;

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
}

int main() {
    test_sequence1();
    // add more test sequences or extend the existing one

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
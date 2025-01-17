// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. run the test by running ./bm_test.exe

#include <cassert>
#include <iostream>

#include "amk_block.hpp"

void test_sequence1() {
    // the tests should show that your code matches the simulink model's
    // expected behavior
    AmkBlock amk(AmkStates::MOTOR_OFF_WAITING_FOR_GOV);
    int time_ms = 0;

    AmkOutput output1 = amk.update(
        AmkInput{
            .cmd = MiCmd::STARTUP,
            .amk_actual_values_1_left = generated::can::AMK0_ActualValues1{},
            .amk_actual_values_2_left = generated::can::AMK0_ActualValues2{},
            .amk_actual_values_1_right = generated::can::AMK1_ActualValues1{},
            .amk_actual_values_2_right = generated::can::AMK1_ActualValues2{},
            .left_motor_input = MotorInput{},
            .right_motor_input = MotorInput{}},
        time_ms);

    assert(output1.status == MiStatus::OFF);

    time_ms += 10;  // next update comes 10 ms later

    AmkOutput output2 = amk.update(
        // this input matches the condition to move from InitialState to
        // StartupState1 in Simulink
        AmkInput{
            .cmd = MiCmd::STARTUP,
            .amk_actual_values_1_left = generated::can::AMK0_ActualValues1{},
            .amk_actual_values_2_left = generated::can::AMK0_ActualValues2{},
            .amk_actual_values_1_right = generated::can::AMK1_ActualValues1{},
            .amk_actual_values_2_right = generated::can::AMK1_ActualValues2{},
            .left_motor_input = MotorInput{},
            .right_motor_input = MotorInput{}},
        time_ms);

    assert(output2.status == MiStatus::STARTUP);

    // extend the test sequence to cover more states and transitions
}

int run_tests() {
    test_sequence1();
    // add more test sequences or extend the existing one

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
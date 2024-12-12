// To run these tests, cd into this directory and run:
// make
// this will compile it to bm_test.exe. run the test by running ./bm_test.exe

#include "amk_block.hpp"

#include <cassert>
#include <iostream>

void test_sequence1() {
    // the tests should show that your code matches the simulink model's
    // expected behavior
    AmkBlock amk{};
    int time_ms = 0;

    AmkOutput output1 = amk.update(
        AmkInput{

        }
        time_ms);

    assert(output1.status == MiStatus::OFF);

    time_ms += 10;  // next update comes 10 ms later

    AmkOutput output2 = amk.update(
        // this input matches the condition to move from InitialState to
        // StartupState1 in Simulink
        AmkInput{
            
        },
        time_ms);

    assert(output2.status == MiStatus::IDLE);

    // extend the test sequence to cover more states and transitions
}

int main() {
    test_sequence1();
    // add more test sequences or extend the existing one

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
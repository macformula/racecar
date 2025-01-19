#include <iostream>

// #include "driver_interface.hpp"
#include "shared/controls/testing.h"

void test_example() {
    ASSERT_TRUE(1 + 1 == 2);

    // uncomment to see what happens when a test fails
    // ASSERT_TRUE(1 + 1 == 3);
}

void test_brake_light() {
    // The brake light should be activate if brake pedal pos > 10%
    // DriverInterface driver_interface;

    // {
    //     DiInput in = {
    //         .brake_pedal_position = 0.5,
    //     };
    //     DiOutput out = di.Update(in, 0);
    //     // ASSERT_TRUE(out.brake_light_enable);
    // }

    // {
    //     DiInput in = {
    //         .brake_pedal_position = 0.08,
    //     };
    //     DiOutput out = di.Update(in, 0);
    //     // ASSERT_FALSE(out.brake_light_enable);
    // }
}

int main() {
    test_example();
    test_brake_light();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
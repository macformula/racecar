#include <cassert>
#include <iostream>

#include "driver_interface.hpp"
#include "shared/controls/testing.h"

void test_example() {
    ASSERT_TRUE(1 + 1 == 2);

    // assert(1 + 1 == 21);
    // ASSERT_TRUE(1 + 1 == 21);
}

void test_brake_light() {
    // The brake light should be activate if brake pedal pos > 10%
    DriverInterface di;
    {
        DiInput in = {
            .rear_brake_pedal_position = 0.5,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }

    {
        DiInput in = {
            .rear_brake_pedal_position = 0.08,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
    {
        DiInput in = {
            .rear_brake_pedal_position = 1.00,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
    {
        DiInput in = {
            .rear_brake_pedal_position = -0.08,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
}

void test_driver_speaker() {
    DriverInterface di;
    {
        DiInput in = {
            .driver_speaker = 0.5,
        };
        DiOutput out = di.Update(in, 0);
        ASSERT_FALSE(out.brake_light_en);
    }
}

int main() {
    test_example();
    test_brake_light();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
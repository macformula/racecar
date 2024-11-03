#include "brake_pedal_lights.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test Case 1: Brake Pedal Map Edge Cases
    {
        float result = brakePedalMap(0.0f);
        ASSERT_CLOSE(result, 0.0f);

        result = brakePedalMap(100.0f);
        ASSERT_CLOSE(result, 100.0f);

        result = brakePedalMap(50.0f);
        ASSERT_CLOSE(result, 50.0f);
    }

    // Test Case 2: Brake Pedal Map with Interpolated Value
    {
        float result = brakePedalMap(12.5f);
        ASSERT_CLOSE(result, 12.5f);
    }

    // Test Case 3: brake_pedal_lights with No Errors and High Pedal Positions
    {
        auto [brakePedalPos, brakeLightEn] =
            brake_pedal_lights(false, false, 0.8f, 0.9f);
        ASSERT_CLOSE(brakePedalPos, 0.9f);
        ASSERT_TRUE(brakeLightEn);
    }

    // Test Case 4: brake_pedal_lights with No Errors and Low Pedal Positions
    {
        auto [brakePedalPos, brakeLightEn] =
            brake_pedal_lights(false, false, 0.05f, 0.07f);
        ASSERT_CLOSE(brakePedalPos, 0.07f);
        ASSERT_FALSE(brakeLightEn);
    }

    // Test Case 5: brake_pedal_lights with Rear Brake Pedal Error
    {
        auto [brakePedalPos, brakeLightEn] =
            brake_pedal_lights(true, false, 0.8f, 0.9f);
        ASSERT_CLOSE(brakePedalPos, 0.0f);
        ASSERT_FALSE(brakeLightEn);
    }

    // Test Case 6: brake_pedal_lights with Front Brake Pedal Error
    {
        auto [brakePedalPos, brakeLightEn] =
            brake_pedal_lights(false, true, 0.8f, 0.9f);
        ASSERT_CLOSE(brakePedalPos, 0.0f);
        ASSERT_FALSE(brakeLightEn);
    }

    // Test Case 7: brake_pedal_lights with Both Brake Pedal Errors
    {
        auto [brakePedalPos, brakeLightEn] =
            brake_pedal_lights(true, true, 0.8f, 0.9f);
        ASSERT_CLOSE(brakePedalPos, 0.0f);
        ASSERT_FALSE(brakeLightEn);
    }

    // Test Case 8: brake_pedal_lights with Front Pedal Just Above Threshold
    {
        auto [brakePedalPos, brakeLightEn] =
            brake_pedal_lights(false, false, 0.09f, 0.11f);
        ASSERT_CLOSE(brakePedalPos, 0.11f);
        ASSERT_TRUE(brakeLightEn);
    }

    std::cout << "All test cases passed!" << std::endl;

    return 0;
}
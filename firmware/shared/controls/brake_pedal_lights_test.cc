#include "brake_pedal_lights.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test Case 1: CalculateBrakePedalPosition with No Errors and Edge Values
    ASSERT_CLOSE(CalculateBrakePedalPosition(false, false, 0.0f, 0.0f), 0.0f);

    ASSERT_CLOSE(CalculateBrakePedalPosition(false, false, 1.0f, 0.0f), 1.0f);

    ASSERT_CLOSE(CalculateBrakePedalPosition(false, false, 0.5f, 0.7f), 0.7f);

    // Test Case 2: CalculateBrakePedalPosition with Brake Error
    ASSERT_CLOSE(CalculateBrakePedalPosition(true, false, 0.8f, 0.9f), 0.0f);

    ASSERT_CLOSE(CalculateBrakePedalPosition(false, true, 0.8f, 0.9f), 0.0f);

    ASSERT_CLOSE(CalculateBrakePedalPosition(true, true, 0.8f, 0.9f), 0.0f);

    // Test Case 3: DetectBrakeLight with Values Just Below and Above the
    // Threshold
    ASSERT_FALSE(DetectBrakeLight(0.05f));  // Below threshold

    ASSERT_FALSE(DetectBrakeLight(0.09f));  // Near threshold

    ASSERT_TRUE(DetectBrakeLight(0.11f));  // Just above threshold

    // Test Case 4: DetectBrakeLight with High Pedal Positions
    ASSERT_TRUE(DetectBrakeLight(0.5f));

    ASSERT_TRUE(DetectBrakeLight(1.0f));

    std::cout << "All test cases passed!" << std::endl;

    return 0;
}
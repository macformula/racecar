#include "steering_angle.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test case 1: No error expected
    ASSERT_CLOSE(CalculateSteeringAngle(false, 0.8), 0.8);

    // Test case 2: No error expected
    ASSERT_CLOSE(CalculateSteeringAngle(false, 1.0), 1.0);

    // Test case 3: No error expected
    ASSERT_CLOSE(CalculateSteeringAngle(false, 0.0), 0.0);

    // Test case 4: No error expected
    ASSERT_CLOSE(CalculateSteeringAngle(false, 0.5), 0.5);

    // Test case 5: Error, steering angle should be reset to 0.5
    ASSERT_CLOSE(CalculateSteeringAngle(true, 0.8), 0.5);

    // Test case 6: Error, steering angle should be reset to 0.5
    ASSERT_CLOSE(CalculateSteeringAngle(true, 0.2), 0.5);

    // Test case 7: Error, steering angle should be reset to 0.5
    ASSERT_CLOSE(CalculateSteeringAngle(true, 0.5), 0.5);

    std::cout << "All test cases passed!" << std::endl;

    return 0;
}
#include "driver_interface_error_handling.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    // Test case 1: No error expected
    ASSERT_FALSE(b_DriverInterfaceError(0.8, 0.85, 0.9, 0.95, 0.5));

    // Test case 2, No error expected
    ASSERT_FALSE(b_DriverInterfaceError(0.52, 0.48, 0.02, 0.33, 0.54));

    // Test case 3: No error expected
    ASSERT_FALSE(b_DriverInterfaceError(0.75, 0.68, 0.34, 0.62, 0.3));

    // Test case 4: No error expected
    ASSERT_FALSE(b_DriverInterfaceError(0.34, 0.28, 0.82, 0.73, 0.84));

    // Test case 5: AccelPedalPos1 out of range
    ASSERT_TRUE(b_DriverInterfaceError(1.2, 0.8, 0.9, 0.7, 0.5));

    // Test case 6: AccelPedalPos2 out of range
    ASSERT_TRUE(b_DriverInterfaceError(0.8, 1.2, 0.9, 0.7, 0.5));

    // Test case 7: FrontBrakePressure out of range
    ASSERT_TRUE(b_DriverInterfaceError(0.8, 0.7, 1.1, 0.8, 0.5));

    // Test case 8: RearBrakePressure out of range
    ASSERT_TRUE(b_DriverInterfaceError(0.81, 0.76, 0.9, -0.1, 0.5));

    // Test case 9: RawSteeringAngle out of range
    ASSERT_TRUE(b_DriverInterfaceError(0.86, 0.78, 0.9, 0.8, -0.5));

    // Test case 10: Difference between PedalPos1 and PedalPos2 exceeds
    // threshold
    ASSERT_TRUE(b_DriverInterfaceError(0.8, 0.95, 0.9, 0.8, 0.5));

    // Test case 11: Pedal position and parameters should result in true
    ASSERT_TRUE(b_DriverInterfaceError(0.25, 0.4, 0.9, 0.8, 0.5));

    // Test case 12, fails because pedal position is > 0.1
    ASSERT_TRUE(b_DriverInterfaceError(0.5, 0.61, 0.5, 0.5, 0.5));

    std::cout << "All test cases passed!" << std::endl;
    return 0;
}
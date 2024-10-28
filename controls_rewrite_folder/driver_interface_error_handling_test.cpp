#include "driver_interface_error_handling.h"
#include "driver_interface_error_handling_testing.h"
#include <iostream>


int main() {
    using namespace ctrl;

    // Test case 1: No error expected
    ASSERT_EQ(b_DriverInterfaceError(0.8, 0.85, 0.9, 0.95, 0.5), false);

    // Test case 2: AccelPedalPos1 out of range
    ASSERT_EQ(b_DriverInterfaceError(1.2, 0.8, 0.9, 0.7, 0.5), true);

    // Test case 3: AccelPedalPos2 out of range
    ASSERT_EQ(b_DriverInterfaceError(0.8, 1.2, 0.9, 0.7, 0.5), true);

    // Test case 4: FrontBrakePressure out of range
    ASSERT_EQ(b_DriverInterfaceError(0.8, 0.7, 1.1, 0.8, 0.5), true);

    // Test case 5: RearBrakePressure out of range
    ASSERT_EQ(b_DriverInterfaceError(0.8, 0.7, 0.9, -0.1, 0.5), true);

    // Test case 6: RawSteeringAngle out of range
    ASSERT_EQ(b_DriverInterfaceError(0.8, 0.7, 0.9, 0.8, -0.5), true);

    // Test case 7: Difference between PedalPos1 and PedalPos2 exceeds threshold
    ASSERT_EQ(b_DriverInterfaceError(0.8, 0.95, 0.9, 0.8, 0.5), true);

    // Test case 8: Pedal positions difference exactly on threshold
    ASSERT_EQ(b_DriverInterfaceError(0.8, 0.9, 0.9, 0.8, 0.5), false);

    // Test case 9: Pedal position and parameters should NOT result in true
    ASSERT_NEQ(b_DriverInterfaceError(0.3, 0.4, 0.9, 0.8, 0.5), true);

    // Test case 10: Pedal position and parameters should NOT result in true
    ASSERT_NEQ(b_DriverInterfaceError(0.5, 0.4, 0.2, 0.3, 0.5), true);

    // Test case 11: Pedal position and parameters should NOT result in false
    ASSERT_NEQ(b_DriverInterfaceError(0.25, 0.4, 0.9, 0.8, 0.5), false);

    // Test case 12: Pedal position and parameters should NOT result in true
    ASSERT_NEQ(b_DriverInterfaceError(0, 0, 0, 0, 0), true);

    // Test case 13: Pedal position and parameters should result in false
    ASSERT_EQ(b_DriverInterfaceError(0, 0, 0, 0, 0), false);

    // Test case 14: Pedal position and parameters should NOT result in true
    ASSERT_NEQ(b_DriverInterfaceError(1.0, 1.0, 1.0, 1.0, 1.0), true);

    // Test case 15: Pedal position and parameters should result in false
    ASSERT_EQ(b_DriverInterfaceError(1.0, 1.0, 1.0, 1.0, 1.0), false);

    // Test case 16: Pedal position out of bounds, should return in true
    ASSERT_EQ(b_DriverInterfaceError(1.5, 1.6, 1.0, 1.0, 1.0), true);

    std::cout << "All test cases passed!" << std::endl;    
    return 0;
}
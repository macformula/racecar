#include <iostream> 

#include "tvFactor.h"

#include "testing.h"

int main() {
    using namespace ctrl;

    ASSERT_CLOSE(
        EnableTorqueVectoring(26.3, 0.2),
        26.3);  // Should return 30.0 because DI_b_EnableTorqueVectoring > 0
    ASSERT_CLOSE(
        EnableTorqueVectoring(21.3, 0.0),
        0.0);  // Should return 0.0 because DI_b_EnableTorqueVectoring <= 0
    ASSERT_CLOSE(
        EnableTorqueVectoring(-20.0, 1.5),
        -20.0);  // Should return -20.0 because DI_b_EnableTorqueVectoring > 0
    ASSERT_CLOSE(
        EnableTorqueVectoring(12.0, -6.2),
        0.0);  // Should return 0.0 because DI_b_EnableTorqueVectoring <= 0

    ASSERT_CLOSE(CreateTorqueVectoringFactor(5.0), 0.934);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(10.0), 0.87);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(13.2), 0.83032);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(23.3), 0.70476);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(27.3), 0.683);

    auto [leftFactor1, rightFactor1] = AdjustTorqueVectoring(15.2, 3.6);
    ASSERT_CLOSE(leftFactor1, 1.0); 
    ASSERT_CLOSE(rightFactor1, 3.6);

    auto [leftFactor2, rightFactor2] = AdjustTorqueVectoring(-13.6, 2.2);
    ASSERT_CLOSE(leftFactor2, 2.2);
    ASSERT_CLOSE(rightFactor2, 1);

    auto [leftFactor3, rightFactor3] = AdjustTorqueVectoring(0.0, 6.0);
    ASSERT_CLOSE(leftFactor3, 1);
    ASSERT_CLOSE(rightFactor3, 1);

    std::cout << "All tests passed" << std::endl;
    
    return 0;
}

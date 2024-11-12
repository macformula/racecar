#include <iostream> 

#include "tvFactor.h"

#include "testing.h"

int main() {
    using namespace ctrl;

    ASSERT_CLOSE(CreateTorqueVectoringFactor(5.0), 0.934);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(10.0), 0.87);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(13.2), 0.83032);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(23.3), 0.70476);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(27.3), 0.683);

    {
        auto [left_factor, right_factor] = AdjustTorqueVectoring(15.2, 3.6);
        ASSERT_CLOSE(left_factor, 1.0);
        ASSERT_CLOSE(right_factor, 3.6);
    }

    {
        auto [left_factor, right_factor] = AdjustTorqueVectoring(-13.6, 2.2);
        ASSERT_CLOSE(left_factor, 2.2);
        ASSERT_CLOSE(right_factor, 1);
    }

    {
        auto [left_factor, right_factor] = AdjustTorqueVectoring(0.0, 6.0);
        ASSERT_CLOSE(left_factor, 1);
        ASSERT_CLOSE(right_factor, 1);
    }

    std::cout << "All tests passed" << std::endl;
    
    return 0;
}

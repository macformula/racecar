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
        TorqueVector<double> tv = AdjustTorqueVectoring(15.2);
        ASSERT_CLOSE(tv.left_torque_vector, 1.0);
        ASSERT_CLOSE(tv.right_torque_vector, 3.6);
    }

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(-13.6);
        ASSERT_CLOSE(tv.left_torque_vector, 2.2);
        ASSERT_CLOSE(tv.right_torque_vector, 1);
    }

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(0.0);
        ASSERT_CLOSE(tv.left_torque_vector, 1);
        ASSERT_CLOSE(tv.right_torque_vector, 1);
    }

    std::cout << "All tests passed" << std::endl;
    
    return 0;
}

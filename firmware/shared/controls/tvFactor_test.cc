#include "tvFactor.h"

#include <iostream>

#include "testing.h"

int main() {
    using namespace ctrl;

    ASSERT_CLOSE(CreateTorqueVectoringFactor(5.0), 0.934);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(10.0), 0.87);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(13.2), 0.83032);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(23.3), 0.70476);
    ASSERT_CLOSE(CreateTorqueVectoringFactor(27.3), 0.683);

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(15.0);
        ASSERT_CLOSE(tv.left, 1.0);
        ASSERT_CLOSE(tv.right, 0.808);
    }

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(-10.0);
        ASSERT_CLOSE(tv.left, 0.87);
        ASSERT_CLOSE(tv.right, 1);
    }

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(0.0);
        ASSERT_CLOSE(tv.left, 1);
        ASSERT_CLOSE(tv.right, 1);
    }

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(13.8);
        ASSERT_CLOSE(tv.left, 1);
        ASSERT_CLOSE(tv.right, 0.82288);
    }

    {
        TorqueVector<double> tv = AdjustTorqueVectoring(-6.7);
        ASSERT_CLOSE(tv.left, 0.91224);
        ASSERT_CLOSE(tv.right, 1);
    }

    std::cout << "All tests passed" << std::endl;

    return 0;
}

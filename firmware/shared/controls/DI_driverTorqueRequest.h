#pragma once
#include <cmath>
#include <iostream>

namespace ctrl {

// Templating means this function will work on any types that support addition.
template <typename T, typename B>
T DI_driverTorqueRequest(T READY_TO_DRIVE, T DI_Error, B AccelPedPos) {
    T u1 = 0.0;
    if ((!READY_TO_DRIVE) == 1 || DI_Error == 1) {
        return u1;
    } else {
        // AccelPedPos (n-d Lookup Table)
        if (AccelPedPos > 1) {
            return 1.0;
        } else if (AccelPedPos >= 0) {
            B r_AccelPedPos = round(AccelPedPos);
            return r_AccelPedPos;
        } else {
            return u1;
        }
    }
}

}  // namespace ctrl
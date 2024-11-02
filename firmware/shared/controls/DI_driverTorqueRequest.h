#pragma once
#include <cmath>
namespace ctrl {

// Templating means this function will work on any types that support addition.
template <typename B>
B DI_driverTorqueRequest(bool READY_TO_DRIVE, bool DI_Error, B AccelPedPos) {
    if ((!READY_TO_DRIVE) || DI_Error) {
        return 0;
    } else {
        return AccelPedPos;
    }
}

}  // namespace ctrl
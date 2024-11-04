#include <iostream>
#pragma once

// Peter Jabra

namespace ctrl {

template <typename T>
T CalculateSteeringAngle(bool steering_error, T steerin_angle) {
    // Steering Angle Handling
    if (steering_error) {
        return static_cast<T>(0.5);
    } else {
        return steerin_angle;
    }
}

}  // namespace ctrl
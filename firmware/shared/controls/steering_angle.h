#pragma once

// Peter Jabra

namespace ctrl {

template <typename T>
T CalculateSteeringAngle(bool steering_error, T steering_angle) {
    if (steering_error) {
        return static_cast<T>(0.5);
    } else {
        return steering_angle;
    }
}

}  // namespace ctrl
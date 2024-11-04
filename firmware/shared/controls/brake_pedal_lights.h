#pragma once
#include <algorithm>
#include <iostream>

namespace ctrl {

template <typename T>
T CalculateBrakePedalPosition(bool rear_brake_pedal_error,
                              bool front_brake_pedal_error,
                              T rear_brake_pedal_position,
                              T front_brake_pedal_position) {
    // Brake Error Handling
    if (rear_brake_pedal_error || front_brake_pedal_error) {
        return static_cast<T>(0);
    } else {
        return std::max(front_brake_pedal_position, rear_brake_pedal_position);
    }
}

template <typename T>
bool DetectBrakeLight(T brake_pedal_position) {
    return (brake_pedal_position > 0.1);
}

}  // namespace ctrl
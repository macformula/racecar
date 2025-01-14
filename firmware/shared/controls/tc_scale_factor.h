#pragma once

#include "shared/util/mappers/clamper.hpp"

namespace ctrl {
// Note: The CalculateActualSlip function has Div-by-Zero error if left front
// and right front wheel speeds = 0.
template <typename T>
T CalculateActualSlip(T left_rear_wheel_speed, T right_rear_wheel_speed,
                      T left_front_wheel_speed, T right_front_wheel_speed) {
    T idle_wheel_spd = (left_front_wheel_speed + right_front_wheel_speed) / 2.0;
    T actual_slip;

    if (left_rear_wheel_speed > right_rear_wheel_speed) {
        actual_slip = (left_rear_wheel_speed / idle_wheel_spd) - 1;
    } else {
        actual_slip = (right_rear_wheel_speed / idle_wheel_spd) - 1;
    }

    if (actual_slip < 0) {
        actual_slip = 0;
    }
    return actual_slip;
}

/// @brief Measures the time for which a boolean condition has been continuously
/// held.
class HoldCondition {
public:
    HoldCondition(int time_ms, bool initial_state = false)
        : reset_time_(time_ms), current_state_(initial_state) {}

    void Update(bool state, int time_ms) {
        if (state != current_state_) {
            reset_time_ = time_ms;
        }
        current_time_ = time_ms;
        current_state_ = state;
    }

    /// @brief Return the number of milliseconds for which `state` has been
    /// continuously held.
    /// @note Returns 0 if `state` does not match the most recent state from
    /// `.Update(state, ...)`
    int HeldForMs(bool state) const {
        if (current_state_ == state) {
            return current_time_ - reset_time_;
        } else {
            return 0;
        }
    }

private:
    int reset_time_;
    int current_time_;
    bool current_state_;
};

template <typename T>
T CalculateTCScaleFactor(T actual_slip, T target_slip, int time_ms) {
    const static T kRampTimeMs = 100.;

    // reset_time initialized to (time_ms - kRampTimeMs) to ensure that the
    // first call to this function will return full torque (scale_factor = 1)
    static int reset_time = time_ms - static_cast<int>(kRampTimeMs);

    static HoldCondition is_slipping(time_ms);
    is_slipping.Update(actual_slip > target_slip, time_ms);

    T scale_factor;
    if (is_slipping.HeldForMs(true) >= 50) {
        // Disable torque so wheels can "catch up" and stop slipping
        scale_factor = 0;
        reset_time = time_ms;
    } else {
        // Ramp up to full torque
        scale_factor = static_cast<T>(time_ms - reset_time) / kRampTimeMs;
        scale_factor = shared::util::Clamper<T>::Evaluate(scale_factor, 0, 1);
    }
    // This method still isn't ideal. It will continue ramping for up to 49 msec
    // of slipping. We should probably have a smarter algorithm that predicts
    // slipping before it happens, or at least is cautious when it starts
    // slipping.

    return scale_factor;
}
}  // namespace ctrl
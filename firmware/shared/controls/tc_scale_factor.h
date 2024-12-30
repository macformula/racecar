#pragma once

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

template <typename T>
T CalculateTCScaleFactor(T actual_slip, T target_slip, int time_ms) {
    // Stateflow: Multi-Stage TC
    static T scale_factor = 1.0f;

    static enum State {
        TC_off,
        TC_on,
        TC_on1,
        TC_on2
    } currentState = TC_off;

    static int state_time_start = time_ms;
    int time_elapsed = time_ms - state_time_start;

    switch (currentState) {
        case TC_off:
            if (time_elapsed >= 50 && actual_slip > target_slip) {
                scale_factor = 0.0f;
                state_time_start = time_ms;
                currentState = TC_on2;
            }
            break;
        case TC_on:
            if (time_elapsed >= 33) {
                scale_factor = 1.0f;
                state_time_start = time_ms;
                currentState = TC_off;
            }
            break;
        case TC_on1:
            if (time_elapsed >= 33) {
                scale_factor = 0.5f;
                state_time_start = time_ms;
                currentState = TC_on;
            }
            break;
        case TC_on2:
            if (time_elapsed >= 33 && actual_slip <= target_slip) {
                scale_factor = 0.25f;
                state_time_start = time_ms;
                currentState = TC_on1;
            }
            break;
    }

    return scale_factor;
}
}  // namespace ctrl
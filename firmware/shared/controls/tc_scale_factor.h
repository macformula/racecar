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
        return 0;
    }
    return actual_slip;
}

template <typename T>
T TCStateflowChart(T actual_slip, T target_slip) {
    // Stateflow: Multi-Stage TC
    return 0;
}

template <typename T>
T CalculateTCScaleFactor(T TC_scale_factor_chart_out) {
    return TC_scale_factor_chart_out;
}
}  // namespace ctrl
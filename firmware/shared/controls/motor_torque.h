#pragma once
#include <tuple>

#include "shared/util/moving_average.hpp"

// Peter Jabra and Aleeza Ali Zar
namespace ctrl {

enum class State {
    Run,
    Stop,
};

template <typename T>
std::tuple<T, T> CalculateMotorTorque(T new_torque_value, T right_factor,
                                      T left_factor, bool reset = false) {
    static shared::util::MovingAverage<T, 10> running_average;

    if (reset) {
        running_average = shared::util::MovingAverage<T, 10>();
    }

    running_average.LoadValue(new_torque_value);

    T running_average_value = running_average.GetValue();

    T right_motor_torque_limit = running_average_value * right_factor;
    T left_motor_torque_limit = running_average_value * left_factor;

    return std::tuple(right_motor_torque_limit, left_motor_torque_limit);
}

template <typename T>
T ComputeTorqueRequest(T driver_torque_request, T brake_pedal_position) {
    static State current_state = State::Stop;
    bool brake_on = brake_pedal_position > static_cast<T>(10);

    bool both_pedals_pressed =
        (driver_torque_request >= static_cast<T>(25) && brake_on);
    bool neither_pedal_pressed =
        driver_torque_request < static_cast<T>(5) && !brake_on;

    if (both_pedals_pressed) {
        current_state = State::Stop;
    }
    if (neither_pedal_pressed) {
        current_state = State::Run;
    }

    T driver_torque;

    switch (current_state) {
        case State::Run:
            driver_torque = driver_torque_request;
            break;
        case State::Stop:
            driver_torque = static_cast<T>(0.0);
            break;
    }

    return driver_torque;
}

}  // namespace ctrl

#pragma once
#include <tuple>

#include "shared/util/moving_average.h"

// Peter Jabra and Aleeza Ali Zar

enum class State {
    Run,
    Stop,
};

namespace ctrl {

template <typename T>
class MotorTorqueCalculator {
public:
    static std::tuple<T, T> CalculateMotorTorque(T new_torque_value,
                                                 T right_factor,
                                                 T left_factor) {
        running_average.LoadValue(new_torque_value);

        T running_average_value = running_average.GetValue();
        T scaled_running_average = running_average_value * 10;

        T right_motor_torque_limit = scaled_running_average * right_factor;
        T left_motor_torque_limit = scaled_running_average * left_factor;

        return std::tuple(right_motor_torque_limit, left_motor_torque_limit);
    }

    static void Reset() {
        running_average = shared::util::MovingAverage<T, 10>();
    }

private:
    static shared::util::MovingAverage<T, 10> running_average;
};

template <typename T>
shared::util::MovingAverage<T, 10> MotorTorqueCalculator<T>::running_average;

template <typename T>
T ComputeTorqueRequest(T driver_torque_request, T brake_pedal_position) {
    static State current_state = State::Stop;
    bool brake_on = brake_pedal_position > static_cast<T>(10);

    bool both_pedals_pressed =
        driver_torque_request >= static_cast<T>(25) && brake_on;
    bool neither_pedal_pressed =
        driver_torque_request < static_cast<T>(5) && !brake_on;

    if (both_pedals_pressed) {
        current_state = State::Stop;
    }
    if (neither_pedal_pressed) {
        current_state = State::Run;
    }

    switch (current_state) {
        case State::Run:
            return driver_torque_request;

        case State::Stop:
            return static_cast<T>(0.0);
        default:
            return static_cast<T>(0.0);
    }
}

}  // namespace ctrl

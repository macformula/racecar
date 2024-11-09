#pragma once
#include <stdbool.h>

#include <queue>
#include <tuple>

// Peter Jabra and Aleeza Ali Zar

enum class State {
    Run,
    Stop,
};

namespace ctrl {

template <typename T>
class RunningAverageCalculator {
public:
    RunningAverageCalculator() : sum(0.0) {}

    T Compute(T new_sample) {
        const int max_samples = 10;

        sum += new_sample;
        samples.push(new_sample);

        if (samples.size() > max_samples) {
            sum -= samples.front();
            samples.pop();
        }

        return sum / samples.size();
    }

    void Reset() {
        sum = 0.0;
        std::queue<T> empty;
        std::swap(samples, empty);
    }

private:
    std::queue<T> samples;
    T sum;
};

template <typename T>
std::tuple<T, T> CalculateMotorTorque(T raw_torque_values[], int size,
                                      T right_factor, T left_factor) {
    T right_motor_torque_limit;
    T left_motor_torque_limit;
    RunningAverageCalculator<T> running_average_calculator;

    for (int i = 0; i < size; i++) {
        T running_average =
            running_average_calculator.Compute(raw_torque_values[i] * 10);

        right_motor_torque_limit = running_average * right_factor;
        left_motor_torque_limit = running_average * left_factor;
    }

    return std::tuple(right_motor_torque_limit, left_motor_torque_limit);
}

template <typename T>
bool CheckBrakePedalPosition(T brake_pedal_position) {
    return brake_pedal_position > static_cast<T>(10);
}

template <typename T>
T ComputeTorqueRequest(T driver_torque_request, T brake_pedal_position) {
    State currentState = State::Stop;
    bool brake_on = CheckBrakePedalPosition<T>(brake_pedal_position);

    T motor_torque_request;

    while (true) {
        switch (currentState) {
            case State::Run:
                motor_torque_request = driver_torque_request;

                if (driver_torque_request >= static_cast<T>(25) && brake_on) {
                    currentState = State::Stop;
                } else {
                    return motor_torque_request;
                }

            case State::Stop:
                motor_torque_request = static_cast<T>(0.0);

                if (driver_torque_request < static_cast<T>(5) && !brake_on) {
                    currentState = State::Run;
                } else {
                    return motor_torque_request;
                }
        }
    }
}

}  // namespace ctrl

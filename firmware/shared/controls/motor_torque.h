#pragma once
#include <tuple>

#include "shared/controls/tvFactor.h"
#include "shared/util/moving_average.hpp"


// Peter Jabra and Aleeza Ali Zar
namespace ctrl {

enum class State {
    Run,
    Stop,
};

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

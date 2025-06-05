#include "driver_interface.hpp"

#include <cmath>

DriverInterface::Output DriverInterface::Update(const Input input,
                                                const int time_ms) {
    const float kPedalImplausiblePercent = 10;
    const int kSpeakerDurationMs = 2000;

    double apps_diff =
        std::abs(input.accel_pedal_pos1 - input.accel_pedal_pos2);
    bool apps_implausible = apps_diff > kPedalImplausiblePercent;

    status_ = Transition(input, apps_implausible, time_ms);

    Output out;
    out.status = status_;
    out.brake_light_en = input.brake_pedal_pos > kBrakePressedThreshold;

    if (status_ == driver_interface::State::RUNNING) {
        out.ready_to_drive = true;
        out.driver_speaker =
            time_ms <= speaker_start_time_ + kSpeakerDurationMs;
        out.driver_torque_req = apps_implausible ? 0 : input.accel_pedal_pos1;
    } else {
        out.ready_to_drive = false;
        out.driver_speaker = false;
        out.driver_torque_req = 0;
    }

    return out;
}

driver_interface::State DriverInterface::Transition(
    const DriverInterface::Input input, bool di_error, const int time_ms) {
    // Superstate transitions
    if (status_ == driver_interface::State::WAITING_FOR_DRIVER_HV ||
        status_ == driver_interface::State::REQUESTED_HV_START ||
        status_ == driver_interface::State::REQUESTED_MOTOR_START ||
        status_ == driver_interface::State::RUNNING) {
        if (input.command == driver_interface::Command::SHUTDOWN) {
            return driver_interface::State::IDLE;
        } else if (di_error) {
            return driver_interface::State::ERR;
        }
    }

    // Regular transitions
    switch (status_) {
        case driver_interface::State::IDLE:
            if (input.command == driver_interface::Command::INIT) {
                return driver_interface::State::WAITING_FOR_DRIVER_HV;
            }
            break;

        case driver_interface::State::WAITING_FOR_DRIVER_HV:
            if (input.dash_cmd == DashState::STARTING_HV) {
                return driver_interface::State::REQUESTED_HV_START;
            }
            break;

        case driver_interface::State::REQUESTED_HV_START:
            if (input.command == driver_interface::Command::HV_IS_ON) {
                return driver_interface::State::WAITING_FOR_DRIVER_MOTOR;
            }
            break;

        case driver_interface::State::WAITING_FOR_DRIVER_MOTOR:
            if (input.dash_cmd == DashState::STARTING_MOTORS) {
                return driver_interface::State::REQUESTED_MOTOR_START;
            }
            break;

        case driver_interface::State::REQUESTED_MOTOR_START:
            if ((input.command == driver_interface::Command::READY_TO_DRIVE) &&
                (input.brake_pedal_pos > kBrakePressedThreshold)) {
                speaker_start_time_ = time_ms;
                return driver_interface::State::RUNNING;
            }
            break;

        case driver_interface::State::RUNNING:
            if (input.command == driver_interface::Command::RUN_ERROR) {
                return driver_interface::State::ERR_COASTING;
            }
            break;

        // No transitions from the following states
        case driver_interface::State::ERR:
            break;
        case driver_interface::State::ERR_COASTING:
            break;
    }
    return status_;  // no transition to perform
}

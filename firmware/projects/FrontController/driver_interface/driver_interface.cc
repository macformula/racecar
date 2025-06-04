#include "driver_interface.hpp"

#include <cmath>

#include "shared/util/lookup_table.hpp"

DriverInterface::Output DriverInterface::Update(const Input input,
                                                const int time_ms) {
    using LUT = shared::util::LookupTable<float>;

    const float kPedalImplausiblePercent = 10;
    const int kSpeakerDurationMs = 2000;

    double apps_diff =
        std::abs(input.accel_pedal_pos1 - input.accel_pedal_pos2);
    bool apps_implausible = apps_diff > kPedalImplausiblePercent;

    status_ = Transition(input, apps_implausible, time_ms);

    Output out;
    out.status = status_;
    out.brake_light_en = input.brake_pedal_pos > kBrakePressedThreshold;

    if (status_ == DiSts::RUNNING) {
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

DiSts DriverInterface::Transition(const DriverInterface::Input input,
                                  bool di_error, const int time_ms) {
    // Superstate transitions
    if (status_ == DiSts::WAITING_FOR_DRIVER_HV ||
        status_ == DiSts::REQUESTED_HV_START ||
        status_ == DiSts::REQUESTED_MOTOR_START || status_ == DiSts::RUNNING) {
        if (input.command == DiCmd::SHUTDOWN) {
            return DiSts::IDLE;
        } else if (di_error) {
            return DiSts::ERR;
        }
    }

    // Regular transitions
    switch (status_) {
        case DiSts::IDLE:
            if (input.command == DiCmd::INIT) {
                return DiSts::WAITING_FOR_DRIVER_HV;
            }
            break;

        case DiSts::WAITING_FOR_DRIVER_HV:
            if (input.dash_cmd == DashState::STARTING_HV) {
                return DiSts::REQUESTED_HV_START;
            }
            break;

        case DiSts::REQUESTED_HV_START:
            if (input.command == DiCmd::HV_IS_ON) {
                return DiSts::WAITING_FOR_DRIVER_MOTOR;
            }
            break;

        case DiSts::WAITING_FOR_DRIVER_MOTOR:
            if (input.dash_cmd == DashState::STARTING_MOTORS) {
                return DiSts::REQUESTED_MOTOR_START;
            }
            break;

        case DiSts::REQUESTED_MOTOR_START:
            if ((input.command == DiCmd::READY_TO_DRIVE) &&
                (input.brake_pedal_pos > kBrakePressedThreshold)) {
                speaker_start_time_ = time_ms;
                return DiSts::RUNNING;
            }
            break;

        case DiSts::RUNNING:
            if (input.command == DiCmd::RUN_ERROR) {
                return DiSts::ERR_COASTING;
            }
            break;

        // No transitions from the following states
        case DiSts::ERR:
            break;
        case DiSts::ERR_COASTING:
            break;
    }
    return status_;  // no transition to perform
}

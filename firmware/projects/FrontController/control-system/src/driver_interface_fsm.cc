#include "control-system/driver_interface_fsm.hpp"

#include "control-system/enums.hpp"

DiFsm::DiFsm(DiSts initial_state) : status_(initial_state) {}

DiFsm::Output DiFsm::Update(const DiFsm::Input input, const int time_ms) {
    const int kSpeakerDurationMs = 2000;
    status_ = Transition(input, time_ms);

    DiFsm::Output out{
        .status = status_,
        .speaker_enable = false,
        .ready_to_drive = false,
    };

    // The output is only interesting in the RUNNING state
    if (status_ == DiSts::RUNNING) {
        out.ready_to_drive = true;
        out.speaker_enable =
            time_ms <= speaker_start_time_ + kSpeakerDurationMs;
    };

    return out;
}

DiSts DiFsm::Transition(const DiFsm::Input input, const int time_ms) {
    // Superstate transitions
    if (status_ == DiSts::WAITING_FOR_DRVR || status_ == DiSts::HV_START_REQ ||
        status_ == DiSts::MOTOR_START_REQ || status_ == DiSts::RUNNING) {
        if (input.command == DiCmd::SHUTDOWN) {
            return DiSts::INIT;
        } else if (input.di_error) {
            return DiSts::ERR;
        }
    }

    // Regular transitions
    switch (status_) {
        case DiSts::INIT:
            if (input.command == DiCmd::INIT) {
                return DiSts::WAITING_FOR_DRVR;
            }
            break;

        case DiSts::WAITING_FOR_DRVR:
            if (input.driver_button) {
                return DiSts::HV_START_REQ;
            }
            break;

        case DiSts::HV_START_REQ:
            if (input.command == DiCmd::HV_ON && input.driver_button) {
                return DiSts::MOTOR_START_REQ;
            }
            break;

        case DiSts::MOTOR_START_REQ:
            if (input.command == DiCmd::READY_TO_DRIVE &&
                input.brake_pedal_pos > 0.1) {
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
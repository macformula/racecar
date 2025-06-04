#include "governor.hpp"

#include <optional>

#include "../enums.hpp"

Governor::Governor() : fsm_state_(std::nullopt) {}

Governor::Output Governor::Update(const Governor::Input input,
                                  const int time_ms) {
    using enum GovSts;

    auto transition = Transition(input, time_ms);

    if (!transition.has_value()) {
        return output_;  // no transition -> nothing to update
    }

    fsm_state_ = transition.value();
    state_entered_time_ = time_ms;

    switch (*fsm_state_) {
        case INIT:
            output_.bm_cmd = BmCmd::INIT;
            output_.mi_cmd = MiCmd::INIT;
            output_.di_cmd = DiCmd::INIT;
            motor_start_count_ = 0;
            break;

        case STARTUP_HV:
            output_.bm_cmd = BmCmd::STARTUP;
            break;

        case STARTUP_READY_TO_DRIVE:
            output_.di_cmd = DiCmd::HV_IS_ON;
            break;

        case STARTUP_MOTOR:
            output_.mi_cmd = MiCmd::STARTUP;
            motor_start_count_++;
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            output_.di_cmd = DiCmd::READY_TO_DRIVE;
            break;

        case RUNNING:
            break;

        case SHUTDOWN:
            output_.mi_cmd = MiCmd::SHUTDOWN;
            output_.di_cmd = DiCmd::SHUTDOWN;
            break;

        case ERR_STARTUP_HV:
            output_.di_cmd = DiCmd::SHUTDOWN;
            break;

        case ERR_STARTUP_MOTOR_RESET:
            output_.mi_cmd = MiCmd::ERR_RESET;
            break;

        case ERR_STARTUP_MOTOR_FAULT:
            output_.mi_cmd = MiCmd::SHUTDOWN;
            break;

        case ERR_STARTUP_DI:
            break;

        case ERR_RUNNING_HV:
            output_.di_cmd = DiCmd::RUN_ERROR;
            break;

        case ERR_RUNNING_MOTOR:
            output_.di_cmd = DiCmd::RUN_ERROR;
            break;
    }

    output_.gov_sts = *fsm_state_;

    return output_;
}

std::optional<GovSts> Governor::Transition(const Governor::Input input,
                                           const int time_ms) {
    using enum GovSts;

    if (!fsm_state_.has_value()) {
        // this will happen only the first cycle.
        return INIT;
    }

    // HVIL_INTERRUPT is not part of BM anymore. What should this condition be?
    bool hvil_interrupt = false;

    // Handle the startup superstate errors
    if (*fsm_state_ == STARTUP_HV || *fsm_state_ == STARTUP_READY_TO_DRIVE ||
        *fsm_state_ == STARTUP_MOTOR ||
        *fsm_state_ == STARTUP_SEND_READY_TO_DRIVE) {
        if (hvil_interrupt) {
            return ERR_STARTUP_HV;
        }

        if (input.mi_sts == MiSts::ERR) {
            if (motor_start_count_ < kMaxMotorStartAttempts) {
                return ERR_STARTUP_MOTOR_RESET;
            } else {
                return ERR_STARTUP_MOTOR_FAULT;  // don't keep trying
            }
        }

        if (input.di_sts == DiSts::ERR) {
            return ERR_STARTUP_DI;
        }
    }

    switch (*fsm_state_) {
        case INIT:
            if (time_ms - state_entered_time_ >= 2000 &&
                input.di_sts == DiSts::REQUESTED_HV_START) {
                return STARTUP_HV;
            }
            break;

        case STARTUP_HV:
            if (input.bm_sts == BmSts::RUNNING) {
                return STARTUP_READY_TO_DRIVE;
            }
            break;

        case STARTUP_READY_TO_DRIVE:
            if (input.di_sts == DiSts::REQUESTED_MOTOR_START) {
                return STARTUP_MOTOR;
            }
            break;

        case STARTUP_MOTOR:
            if (input.mi_sts == MiSts::RUNNING) {
                return STARTUP_SEND_READY_TO_DRIVE;
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            if (input.di_sts == DiSts::RUNNING) {
                return RUNNING;
            }
            break;

        case RUNNING:
            if (hvil_interrupt || input.bm_sts == BmSts::LOW_SOC) {
                return SHUTDOWN;
            }

            if (input.bm_sts == BmSts::ERR_RUNNING) {
                return ERR_RUNNING_HV;
            }

            if (input.mi_sts == MiSts::ERR) {
                return ERR_RUNNING_MOTOR;
            }

            break;

        case SHUTDOWN:
            if (input.bm_sts == BmSts::INIT) {
                return INIT;
            }
            break;

        case ERR_STARTUP_HV:
            if (input.bm_sts == BmSts::INIT) {
                return INIT;
            }
            break;

        case ERR_STARTUP_MOTOR_RESET:
            if (input.mi_sts == MiSts::OFF) {
                return STARTUP_HV;
            }
            break;

        // The vehicle cannot escape these error states without rebooting.
        case ERR_STARTUP_MOTOR_FAULT:
            break;
        case ERR_STARTUP_DI:
            break;
        case ERR_RUNNING_HV:
            break;
        case ERR_RUNNING_MOTOR:
            break;
    }

    return std::nullopt;
}
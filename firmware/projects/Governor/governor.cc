#include <optional>

#include "governor.hpp"

Governor::Governor() : is_first_update_(true) {
    state_.gov_sts = GovSts::INIT;
}

Governor::State Governor::Update(const Governor::Input input,
                                 const int time_ms) {
    using enum GovSts;

    auto new_state = Transition(input, time_ms);

    if (!new_state.has_value()) {
        // no transition -> nothing to update
        return state_;
    }

    state_.gov_sts = new_state.value();
    state_entered_time_ = time_ms;

    switch (state_.gov_sts) {
        case INIT:
            state_.bm_cmd = BmCmd::INIT;
            state_.di_cmd = DiCmd::INIT;
            motor_start_count_ = 0;
            break;

        case STARTUP_HV:
            state_.bm_cmd = BmCmd::HV_STARTUP;
            break;

        case STARTUP_READY_TO_DRIVE:
            state_.di_cmd = DiCmd::HV_ON;
            break;

        case STARTUP_MOTOR:
            state_.mi_cmd = MiCmd::STARTUP;
            motor_start_count_++;
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            state_.di_cmd = DiCmd::READY_TO_DRIVE;
            break;

        case RUNNING:
            break;

        case SHUTDOWN:
            state_.mi_cmd = MiCmd::SHUTDOWN;
            state_.di_cmd = DiCmd::SHUTDOWN;
            break;

        case ERR_STARTUP_HV:
            state_.di_cmd = DiCmd::SHUTDOWN;
            break;

        case ERR_STARTUP_MOTOR_RESET:
            state_.mi_cmd = MiCmd::ERR_RESET;
            break;

        case ERR_STARTUP_MOTOR_FAULT:
            state_.mi_cmd = MiCmd::SHUTDOWN;
            break;

        case ERR_STARTUP_DI:
            break;

        case ERR_RUNNING_HV:
            state_.di_cmd = DiCmd::ERR_RUN;
            break;

        case ERR_RUNNING_MOTOR:
            state_.di_cmd = DiCmd::ERR_RUN;
            break;
    }

    return state_;
}

std::optional<GovSts> Governor::Transition(const Governor::Input input,
                                           const int time_ms) {
    using enum GovSts;

    // Handle the startup superstate errors
    if (state_.gov_sts == STARTUP_HV ||
        state_.gov_sts == STARTUP_READY_TO_DRIVE ||
        state_.gov_sts == STARTUP_MOTOR ||
        state_.gov_sts == STARTUP_SEND_READY_TO_DRIVE) {
        if (input.bm_sts == BmSts::HVIL_INTERRUPT) {
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

    switch (state_.gov_sts) {
        case INIT:
            if (time_ms - state_entered_time_ >= 2000 &&
                input.di_sts == DiSts::HV_START_REQ) {
                return STARTUP_HV;
            }
            break;

        case STARTUP_HV:
            if (input.bm_sts == BmSts::RUNNING) {
                return STARTUP_READY_TO_DRIVE;
            }
            break;

        case STARTUP_READY_TO_DRIVE:
            if (input.di_sts == DiSts::READY_TO_DRIVE_REQ) {
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
            if (input.bm_sts == BmSts::HVIL_INTERRUPT ||
                input.bm_sts == BmSts::LOW_SOC) {
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

    if (is_first_update_) {
        is_first_update_ = false;
        return INIT;
    }

    return std::nullopt;
}
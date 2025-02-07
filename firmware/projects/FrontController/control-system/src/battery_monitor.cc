#include "control-system/battery_monitor.h"

#include <optional>

#include "control-system/enums.hpp"

BatteryMonitor::BatteryMonitor()
    : current_status_(std::nullopt), bm_control_status_(std::nullopt) {}

BatteryMonitor::Output BatteryMonitor::Update(const Input& input, int time_ms) {
    auto new_transition = TransitionStatus(input, time_ms);

    if (new_transition.has_value()) {
        status_snapshot_time_ms_ = time_ms;
        current_status_ = new_transition.value();
    }

    auto new_control_transition =
        TransitionControl(current_status_.value(), time_ms);

    if (new_control_transition.has_value()) {
        control_snapshot_time_ms_ = time_ms;
        bm_control_status_ = new_control_transition.value();
    }

    ContactorCMD contactor_cmd = SelectContactorCmd(bm_control_status_.value());

    return Output{.status = current_status_.value(),
                  .control_status = bm_control_status_.value(),
                  .contactor = contactor_cmd};
}

std::optional<BmSts> BatteryMonitor::TransitionStatus(const Input& input,
                                                      int time_ms) {
    using enum ContactorState;
    using enum BmSts;

    if (!current_status_.has_value()) {
        return INIT;
    }

    if (input.pack_soc < 30) {
        return LOW_SOC;
    } else if (current_status_ != LOW_SOC && input.hvil_status == OPEN) {
        return HVIL_INTERRUPT;
    }

    switch (current_status_.value()) {
        case INIT:
            if (input.cmd == BmCmd::HV_STARTUP &&
                input.precharge_contactor_states == OPEN &&
                input.hv_pos_contactor_states == OPEN &&
                input.hv_neg_contactor_states == OPEN) {
                return IDLE;
            }
            break;

        case IDLE:
            if (input.precharge_contactor_states == OPEN &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == OPEN) {
                return STARTUP;
            }
            break;

        case STARTUP:
            if (input.precharge_contactor_states == CLOSED &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == OPEN) {
                return INIT_PRECHARGE;
            }
            break;

        case INIT_PRECHARGE:
            if (input.precharge_contactor_states == CLOSED &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == CLOSED) {
                return PRECHARGE;
            }

            break;

        case PRECHARGE:
            if (input.precharge_contactor_states == OPEN &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == CLOSED) {
                return RUNNING;
            }

            break;

        case RUNNING:
            if (input.precharge_contactor_states == CLOSED ||
                input.hv_neg_contactor_states == OPEN ||
                input.hv_pos_contactor_states == OPEN) {
                return INIT;
            }
            break;

        case HVIL_INTERRUPT:
            if (time_ms - status_snapshot_time_ms_ >= 5000 &&
                input.hvil_status == CLOSED) {
                return INIT;
            }
            break;

        case LOW_SOC:
            break;

        case ERR_RUNNING:
            break;  // The Simulink model never entered this state! Should
                    // remove but Governor has behaviour which uses it. Uh oh!
    }
    return std::nullopt;
}

std::optional<ControlStatus> BatteryMonitor::TransitionControl(BmSts status,
                                                               int time_ms) {
    using enum ContactorState;
    using enum BmSts;

    if (!bm_control_status_.has_value()) {
        return ControlStatus::STARTUP_CMD;
    }

    if (status == HVIL_INTERRUPT || status == LOW_SOC) {
        return ControlStatus::STARTUP_CMD;
    }

    switch (bm_control_status_.value()) {
        case ControlStatus::STARTUP_CMD:
            if (status == IDLE) {
                return ControlStatus::CLOSE_HV_NEG;
            }
            break;

        case ControlStatus::CLOSE_HV_NEG:

            if (time_ms - control_snapshot_time_ms_ >= 100 &&
                status == STARTUP) {
                return ControlStatus::CLOSE_PRECHARGE;
            }
            break;

        case ControlStatus::CLOSE_PRECHARGE:
            if (time_ms - control_snapshot_time_ms_ >= 6500 &&
                status == INIT_PRECHARGE) {
                return ControlStatus::CLOSE_HV_POS;
            }
            break;

        case ControlStatus::CLOSE_HV_POS:
            if (time_ms - control_snapshot_time_ms_ >= 100 &&
                status == PRECHARGE) {
                return ControlStatus::OPEN_PRECHARGE;
            }

            break;

        case ControlStatus::OPEN_PRECHARGE:
            if (status != RUNNING) {
                return ControlStatus::STARTUP_CMD;
            }
            break;
    }

    return std::nullopt;
}

ContactorCMD BatteryMonitor::SelectContactorCmd(
    ControlStatus bm_control_status_) {
    using enum ContactorState;

    switch (bm_control_status_) {
        case ControlStatus::STARTUP_CMD:
            return ContactorCMD{OPEN, OPEN, OPEN};

        case ControlStatus::CLOSE_HV_NEG:
            return ContactorCMD{OPEN, OPEN, CLOSED};

        case ControlStatus::CLOSE_PRECHARGE:
            return ContactorCMD{CLOSED, OPEN, CLOSED};

        case ControlStatus::CLOSE_HV_POS:
            return ContactorCMD{CLOSED, CLOSED, CLOSED};

        case ControlStatus::OPEN_PRECHARGE:
            return ContactorCMD{OPEN, CLOSED, CLOSED};
    }

    return {};  // switch case shouold return. return needed to satisfy warning
}

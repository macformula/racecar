#include "control-system/battery_monitor.h"

#include <optional>

#include "control-system/enums.hpp"

BatteryMonitor::BatteryMonitor() : current_status_(std::nullopt) {}

BatteryMonitor::Output BatteryMonitor::Update(const Input& input, int time_ms) {
    auto new_transition = TransitionStatus(input, time_ms);

    if (new_transition.has_value()) {
        status_snapshot_time_ms_ = time_ms;
        current_status_ = new_transition.value();
    }

    ContactorCMD contactor_cmd = SelectContactorCmd(current_status_.value());

    return Output{.status = current_status_.value(),
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
    }

    int elapsed = time_ms - status_snapshot_time_ms_;

    switch (current_status_.value()) {
        case INIT:
            if (input.cmd == BmCmd::STARTUP &&
                input.precharge_contactor_states == OPEN &&
                input.pos_contactor_states == OPEN &&
                input.neg_contactor_states == OPEN) {
                return STARTUP_CLOSE_NEG;
            }
            break;
        case STARTUP_CLOSE_NEG:
            if (input.precharge_contactor_states == OPEN &&
                input.pos_contactor_states == OPEN &&
                input.neg_contactor_states == CLOSED) {
                return STARTUP_HOLD_CLOSE_NEG;
            }
            break;

        case STARTUP_HOLD_CLOSE_NEG:
            if (elapsed >= 100) {
                return STARTUP_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_CLOSE_PRECHARGE:
            if (input.precharge_contactor_states == CLOSED &&
                input.neg_contactor_states == CLOSED &&
                input.pos_contactor_states == OPEN) {
                return STARTUP_HOLD_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_HOLD_CLOSE_PRECHARGE:
            if (elapsed >= 10000) {
                return STARTUP_CLOSE_POS;
            }
            break;

        case STARTUP_CLOSE_POS:
            if (input.precharge_contactor_states == CLOSED ||
                input.neg_contactor_states == CLOSED ||
                input.pos_contactor_states == CLOSED) {
                return STARTUP_HOLD_CLOSE_POS;
            }
            break;

        case STARTUP_HOLD_CLOSE_POS:
            if (elapsed >= 100) {
                return STARTUP_OPEN_PRECHARGE;
            }
            break;

        case STARTUP_OPEN_PRECHARGE:
            if (input.precharge_contactor_states == OPEN ||
                input.neg_contactor_states == CLOSED ||
                input.pos_contactor_states == CLOSED) {
                return RUNNING;
            }
            break;

        case RUNNING:
            break;

        case LOW_SOC:
            break;

        case ERR_RUNNING:
            break;  // The Simulink model never entered this state! Should
                    // remove but Governor has behaviour which uses it. Uh oh!
    }
    return std::nullopt;
}

ContactorCMD BatteryMonitor::SelectContactorCmd(BmSts status) {
    using enum ContactorState;
    using enum BmSts;

    switch (status) {
        case INIT:
            return ContactorCMD{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = OPEN,
            };

        case STARTUP_CLOSE_NEG:
        case STARTUP_HOLD_CLOSE_NEG:
            return ContactorCMD{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = CLOSED,
            };

        case STARTUP_CLOSE_PRECHARGE:
        case STARTUP_HOLD_CLOSE_PRECHARGE:
            return ContactorCMD{
                .precharge = CLOSED,
                .positive = OPEN,
                .negative = CLOSED,
            };

        case STARTUP_CLOSE_POS:
        case STARTUP_HOLD_CLOSE_POS:
            return ContactorCMD{
                .precharge = CLOSED,
                .positive = CLOSED,
                .negative = CLOSED,
            };

        case STARTUP_OPEN_PRECHARGE:
        case RUNNING:
            return ContactorCMD{
                .precharge = OPEN,
                .positive = CLOSED,
                .negative = CLOSED,
            };
        case BmSts::LOW_SOC:
            return ContactorCMD{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = OPEN,
            };
        case BmSts::ERR_RUNNING:
            return ContactorCMD{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = OPEN,
            };
    }

    return {};  // switch case shouold return. return needed to satisfy warning
}

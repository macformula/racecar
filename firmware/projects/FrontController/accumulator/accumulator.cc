#include "accumulator.hpp"

#include <optional>

Accumulator::Accumulator() : current_status_(std::nullopt) {}

Accumulator::Output Accumulator::Update(const Input& input, int time_ms) {
    auto new_transition = TransitionStatus(input, time_ms);

    if (new_transition.has_value()) {
        status_snapshot_time_ms_ = time_ms;
        current_status_ = new_transition.value();
    }

    return Output{
        .status = current_status_.value(),
        .command = SelectContactorCmd(current_status_.value()),
    };
}

std::optional<AccSts> Accumulator::TransitionStatus(const Input& input,
                                                    int time_ms) {
    using enum ContactorFeedback::State;
    using enum AccSts;

    if (!current_status_.has_value()) {
        return INIT;
    }

    if (input.pack_soc < 30) {
        return LOW_SOC;
    }

    int elapsed = time_ms - status_snapshot_time_ms_;

    switch (current_status_.value()) {
        case INIT:
            if (input.cmd == AccCmd::STARTUP) {
                return STARTUP_ENSURE_OPEN;
            }
            break;

        case STARTUP_ENSURE_OPEN:
            if (input.feedback.precharge == IS_OPEN &&
                input.feedback.positive == IS_OPEN &&
                input.feedback.negative == IS_OPEN) {
                return STARTUP_CLOSE_NEG;
            }
            break;

        case STARTUP_CLOSE_NEG:
            if (input.feedback.precharge == IS_OPEN &&
                input.feedback.positive == IS_OPEN &&
                input.feedback.negative == IS_CLOSED) {
                return STARTUP_HOLD_CLOSE_NEG;
            }
            break;

        case STARTUP_HOLD_CLOSE_NEG:
            if (elapsed >= 100) {
                return STARTUP_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_CLOSE_PRECHARGE:
            if (input.feedback.precharge == IS_CLOSED &&
                input.feedback.negative == IS_CLOSED &&
                input.feedback.positive == IS_OPEN) {
                return STARTUP_HOLD_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_HOLD_CLOSE_PRECHARGE:
            if (elapsed >= 10000) {
                return STARTUP_CLOSE_POS;
            }
            break;

        case STARTUP_CLOSE_POS:
            if (input.feedback.precharge == IS_CLOSED ||
                input.feedback.negative == IS_CLOSED ||
                input.feedback.positive == IS_CLOSED) {
                return STARTUP_HOLD_CLOSE_POS;
            }
            break;

        case STARTUP_HOLD_CLOSE_POS:
            if (elapsed >= 100) {
                return STARTUP_OPEN_PRECHARGE;
            }
            break;

        case STARTUP_OPEN_PRECHARGE:
            if (input.feedback.precharge == IS_OPEN ||
                input.feedback.negative == IS_CLOSED ||
                input.feedback.positive == IS_CLOSED) {
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

ContactorCommand Accumulator::SelectContactorCmd(AccSts status) {
    using enum ContactorCommand::State;
    using enum AccSts;

    switch (status) {
        case INIT:
        case STARTUP_ENSURE_OPEN:
            return ContactorCommand{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = OPEN,
            };

        case STARTUP_CLOSE_NEG:
        case STARTUP_HOLD_CLOSE_NEG:
            return ContactorCommand{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = CLOSE,
            };

        case STARTUP_CLOSE_PRECHARGE:
        case STARTUP_HOLD_CLOSE_PRECHARGE:
            return ContactorCommand{
                .precharge = CLOSE,
                .positive = OPEN,
                .negative = CLOSE,
            };

        case STARTUP_CLOSE_POS:
        case STARTUP_HOLD_CLOSE_POS:
            return ContactorCommand{
                .precharge = CLOSE,
                .positive = CLOSE,
                .negative = CLOSE,
            };

        case STARTUP_OPEN_PRECHARGE:
        case RUNNING:
            return ContactorCommand{
                .precharge = OPEN,
                .positive = CLOSE,
                .negative = CLOSE,
            };
        case AccSts::LOW_SOC:
            return ContactorCommand{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = OPEN,
            };
        case AccSts::ERR_RUNNING:
            return ContactorCommand{
                .precharge = OPEN,
                .positive = OPEN,
                .negative = OPEN,
            };
    }

    return {};  // switch case shouold return. return needed to satisfy warning
}

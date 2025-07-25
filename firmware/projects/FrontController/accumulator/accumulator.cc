#include "accumulator.hpp"

#include "alerts/alerts.hpp"
#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "shared/util/moving_average.hpp"
#include "thresholds.hpp"

using namespace generated::can;
namespace accumulator {

static State state;
static ContactorCommands contactor_command;
static Command command;

static uint32_t elapsed;

// Pack Voltages

static float pack_voltage;
static uint32_t pack_voltage_timeout;
static bool pack_voltage_valid;

static float precharge_voltage;

static float max_pack_voltage;

State GetState(void) {
    return state;
}

TxContactorCommand GetContactorCommand(void) {
    return TxContactorCommand{
        .pack_positive = static_cast<bool>(contactor_command.positive),
        .pack_precharge = static_cast<bool>(contactor_command.precharge),
        .pack_negative = static_cast<bool>(contactor_command.negative),
    };
}

TxAccumulator_Soc GetDebugMsg(void) {
    return TxAccumulator_Soc{
        .pack_voltage = static_cast<uint8_t>(pack_voltage),
        .precharge_voltage = static_cast<uint16_t>(precharge_voltage),
        .max_pack_voltage = static_cast<uint16_t>(max_pack_voltage),
        .soc_percent = static_cast<uint8_t>(GetSocPercent()),
        .precharge_percent = static_cast<uint8_t>(GetPrechargePercent()),
    };
}

float GetSocPercent(void) {
    return (pack_voltage - threshold::HV_MIN_VOLTAGE) * 100.f /
           (threshold::HV_MAX_VOLTAGE - threshold::HV_MIN_VOLTAGE);
}

float GetPrechargePercent(void) {
    // Has a different denominator than SOC
    return 100.f * precharge_voltage / pack_voltage;
}

void SetCommand(Command _command) {
    command = _command;
}

void Init(void) {
    state = State::IDLE;
    contactor_command = {
        .precharge = ContactorCommand::OPEN,
        .positive = ContactorCommand::OPEN,
        .negative = ContactorCommand::OPEN,
    };
    elapsed = 0;
    max_pack_voltage = 0;
    precharge_voltage = 0;
    pack_voltage = 0;
    pack_voltage_valid = false;
    pack_voltage_timeout = 0;
}

static bool FeedbackMatchesCommand(ContactorCommands cmd,
                                   ContactorFeedbacks fb) {
    using enum ContactorCommand;
    using enum ContactorFeedback;

    bool precharge_match =
        (cmd.precharge == CLOSE) == (fb.precharge == IS_CLOSED);
    bool positive_match = (cmd.positive == CLOSE) == (fb.positive == IS_CLOSED);
    bool negative_match = (cmd.negative == CLOSE) == (fb.negative == IS_CLOSED);

    return precharge_match && positive_match && negative_match;
}

static void MeasureVoltages(VehBus& veh_can) {
    // Precharge
    const float HV_SCALE = (6e6 + 20e3) / (20e3);
    static shared::util::MovingAverage<50> ma;

    ma.LoadValue(bindings::precharge_monitor.ReadVoltage() * HV_SCALE);
    precharge_voltage = ma.GetValue();

    auto msg = veh_can.PopRxPack_SOC();
    auto pack_state = veh_can.PopRxPack_State();
    if (msg.has_value() && pack_state.has_value()) {
        pack_voltage = pack_state->Pack_Inst_Voltage();
        max_pack_voltage = msg->Maximum_Pack_Voltage();
        pack_voltage_valid = true;
        pack_voltage_timeout = 0;
    } else {
        // Invalidate if data stops arriving
        pack_voltage_timeout += 10;
        if (pack_voltage_timeout > timeout::PACK_VOLTAGE_UPDATE) {
            pack_voltage_valid = false;
        }
    }
}

static bool IsPrechargeComplete(void) {
    if (!pack_voltage_valid) {
        return false;
    }

    return GetPrechargePercent() > threshold::PRECHARGE_COMPLETE_PERCENT;
}

static void UpdateStateMachine(ContactorFeedbacks fb) {
    using enum State;
    using enum ContactorFeedback;
    using enum ContactorCommand;

    State new_state = state;
    ContactorCommands cmd;

    switch (state) {
        case IDLE:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};

            if (command == Command::ENABLED) {
                new_state = STARTUP_ENSURE_OPEN;
            }
            break;

        case STARTUP_ENSURE_OPEN:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};

            if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_CLOSE_NEG;
            }
            break;

        case STARTUP_CLOSE_NEG:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = CLOSE};

            if (fb.precharge == IS_CLOSED || fb.positive == IS_CLOSED) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_HOLD_CLOSE_NEG;
            } else {
            }
            break;

        case STARTUP_HOLD_CLOSE_NEG:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = CLOSE};

            if (!FeedbackMatchesCommand(cmd, fb)) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (elapsed >= 100) {
                new_state = STARTUP_CLOSE_PRECHARGE;
            } else {
            }
            break;

        case STARTUP_CLOSE_PRECHARGE:
            cmd = {.precharge = CLOSE, .positive = OPEN, .negative = CLOSE};

            if (fb.positive == IS_CLOSED || fb.negative == IS_OPEN) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_HOLD_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_HOLD_CLOSE_PRECHARGE:
            cmd = {.precharge = CLOSE, .positive = OPEN, .negative = CLOSE};

            if (!FeedbackMatchesCommand(cmd, fb)) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (IsPrechargeComplete() &&
                       (elapsed >= timeout::MIN_PRECHARGE_TIME)) {
                new_state = STARTUP_CLOSE_POS;
            } else {
            }
            break;

        case STARTUP_CLOSE_POS:
            cmd = {.precharge = CLOSE, .positive = CLOSE, .negative = CLOSE};

            if (fb.precharge == IS_OPEN || fb.negative == IS_OPEN) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_HOLD_CLOSE_POS;
            } else {
            }
            break;

        case STARTUP_HOLD_CLOSE_POS:
            cmd = {.precharge = CLOSE, .positive = CLOSE, .negative = CLOSE};

            if (!FeedbackMatchesCommand(cmd, fb)) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (elapsed >= 100) {
                new_state = STARTUP_OPEN_PRECHARGE;
            } else {
            }
            break;

        case STARTUP_OPEN_PRECHARGE:
            cmd = {.precharge = OPEN, .positive = CLOSE, .negative = CLOSE};

            if (fb.positive == IS_OPEN || fb.negative == IS_OPEN) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;

            } else if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = RUNNING;
            } else {
            }
            break;

        case RUNNING:
            cmd = {.precharge = OPEN, .positive = CLOSE, .negative = CLOSE};

            if (!FeedbackMatchesCommand(cmd, fb)) {
                alerts::Get().accumulator_contactor_wrong_state = true;
                new_state = ERROR;
            }
            break;

        case SHUTDOWN:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};

            if ((GetPrechargePercent() < threshold::PACK_SHUTDOWN_PERCENT) &&
                (elapsed > timeout::SHUTDOWN_RESET_DELAY)) {
                new_state = IDLE;
            }
            break;

        case ERROR:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};
            break;
    }

    if (command == Command::OFF && state != IDLE && state != SHUTDOWN) {
        new_state = SHUTDOWN;
    }

    // if (GetSocPercent() < threshold::LOW_VOLTAGE_PERCENT) {
    //     alerts::Get().accumulator_low_soc = true;
    //     new_state = ERROR;
    // } // bypass until BMS is set up for SOC estimation

    contactor_command = cmd;

    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

static std::optional<ContactorFeedbacks> ReadContactorFeedbacks(
    VehBus& veh_can) {
    auto fb_msg = veh_can.GetRxContactor_Feedback();

    if (fb_msg.has_value()) {
        return ContactorFeedbacks{
            .precharge = static_cast<ContactorFeedback>(
                fb_msg->Pack_Precharge_Feedback()),

            .negative = static_cast<ContactorFeedback>(
                fb_msg->Pack_Negative_Feedback()),

            .positive = static_cast<ContactorFeedback>(
                fb_msg->Pack_Positive_Feedback()),
        };
    } else {
        return std::nullopt;
    }
}

void Update_100Hz(VehBus& veh_can) {
    MeasureVoltages(veh_can);

    auto fb = ReadContactorFeedbacks(veh_can);

    if (fb.has_value()) {
        UpdateStateMachine(fb.value());
    }
}

}  // namespace accumulator

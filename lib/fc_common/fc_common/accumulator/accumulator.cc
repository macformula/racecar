#include "accumulator.hpp"

#include "fc_common/alerts/alerts.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "moving_average.hpp"
#include "thresholds.hpp"

using namespace generated::can;
namespace macfe::fc {

using State = generated::can::TxFcStatus::AccumulatorState_t;

State Accumulator::GetState(void) {
    return state;
}

TxContactorCommand Accumulator::GetContactorCommand(void) {
    return TxContactorCommand{
        .pack_positive = static_cast<bool>(contactor_command.positive),
        .pack_precharge = static_cast<bool>(contactor_command.precharge),
        .pack_negative = static_cast<bool>(contactor_command.negative),
    };
}

TxAccumulator_Soc Accumulator::GetDebugMsg(void) {
    return TxAccumulator_Soc{
        .pack_voltage = static_cast<uint8_t>(pack_voltage),
        .precharge_voltage = static_cast<uint16_t>(precharge_voltage),
        .max_pack_voltage = static_cast<uint16_t>(max_pack_voltage),
        .soc_percent = static_cast<uint8_t>(GetSocPercent()),
        .precharge_percent = static_cast<uint8_t>(GetPrechargePercent()),
    };
}

float Accumulator::GetSocPercent(void) {
    return (pack_voltage - threshold::HV_MIN_VOLTAGE) * 100.f /
           (threshold::HV_MAX_VOLTAGE - threshold::HV_MIN_VOLTAGE);
}

float Accumulator::GetPrechargePercent(void) {
    // Has a different denominator than SOC
    return 100.f * precharge_voltage / pack_voltage;
}

void Accumulator::SetCommand(Command _command) {
    command = _command;
}

bool Accumulator::FeedbackMatchesCommand(ContactorCommands cmd,
                                         ContactorFeedbacks fb) {
    using enum ContactorCommand;
    using enum ContactorFeedback;

    bool precharge_match =
        (cmd.precharge == CLOSE) == (fb.precharge == IS_CLOSED);
    bool positive_match = (cmd.positive == CLOSE) == (fb.positive == IS_CLOSED);
    bool negative_match = (cmd.negative == CLOSE) == (fb.negative == IS_CLOSED);

    return precharge_match && positive_match && negative_match;
}

void Accumulator::MeasureVoltages(
    VehBus& veh_can, macfe::periph::AnalogInput& precharge_monitor) {
    // Precharge
    const float HV_SCALE = (6e6 + 20e3) / (20e3);

    ma.LoadValue(precharge_monitor.ReadVoltage() * HV_SCALE);
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

bool Accumulator::IsPrechargeComplete(void) {
    if (!pack_voltage_valid) {
        return false;
    }

    return GetPrechargePercent() > threshold::PRECHARGE_COMPLETE_PERCENT;
}

void Accumulator::UpdateStateMachine(ContactorFeedbacks fb) {
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

std::optional<Accumulator::ContactorFeedbacks>
Accumulator::ReadContactorFeedbacks(VehBus& veh_can) {
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

void Accumulator::Update_100Hz(VehBus& veh_can,
                               macfe::periph::AnalogInput& precharge_monitor) {
    MeasureVoltages(veh_can, precharge_monitor);

    auto fb = ReadContactorFeedbacks(veh_can);

    if (fb.has_value()) {
        UpdateStateMachine(fb.value());
    }
}

}  // namespace macfe::fc

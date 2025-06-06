#include "accumulator.hpp"

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "shared/util/moving_average.hpp"
#include "thresholds.hpp"

using namespace generated::can;
namespace accumulator {

static State state;
static ContactorCommands contactor_command;

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

ContactorCommands GetContactorCommand(void) {
    return contactor_command;
}

float GetSocPercent(void) {
    return 100.f * pack_voltage / max_pack_voltage;
}

float GetPrechargePercent(void) {
    // Has a different denominator than SOC
    return 100.f * precharge_voltage / pack_voltage;
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
    static shared::util::MovingAverage<50, float> ma;

    ma.LoadValue(bindings::precharge_monitor.ReadVoltage() * HV_SCALE);
    precharge_voltage = ma.GetValue();

    auto msg = veh_can.PopRxPack_SOC();
    if (msg.has_value()) {
        pack_voltage = msg->Pack_SOC();
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

static void UpdateStateMachine(Command command, ContactorFeedbacks fb) {
    using enum State;
    using enum ContactorFeedback;
    using enum ContactorCommand;

    State new_state = state;
    ContactorCommands cmd = {
        .precharge = OPEN,
        .positive = OPEN,
        .negative = OPEN,
    };

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

            if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_HOLD_CLOSE_NEG;
            }
            break;

        case STARTUP_HOLD_CLOSE_NEG:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = CLOSE};

            if (elapsed >= 100) {
                new_state = STARTUP_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_CLOSE_PRECHARGE:
            cmd = {.precharge = CLOSE, .positive = OPEN, .negative = CLOSE};

            if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_HOLD_CLOSE_PRECHARGE;
            }
            break;

        case STARTUP_HOLD_CLOSE_PRECHARGE:
            cmd = {.precharge = CLOSE, .positive = OPEN, .negative = CLOSE};

            if (IsPrechargeComplete() &&
                (elapsed >= timeout::MIN_PRECHARGE_TIME)) {
                new_state = STARTUP_CLOSE_POS;
            }
            break;

        case STARTUP_CLOSE_POS:
            cmd = {.precharge = CLOSE, .positive = CLOSE, .negative = CLOSE};

            if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = STARTUP_HOLD_CLOSE_POS;
            }
            break;

        case STARTUP_HOLD_CLOSE_POS:
            cmd = {.precharge = CLOSE, .positive = CLOSE, .negative = CLOSE};

            if (elapsed >= 100) {
                new_state = STARTUP_OPEN_PRECHARGE;
            }
            break;

        case STARTUP_OPEN_PRECHARGE:
            cmd = {.precharge = OPEN, .positive = CLOSE, .negative = CLOSE};

            if (FeedbackMatchesCommand(cmd, fb)) {
                new_state = RUNNING;
            }
            break;

        case RUNNING:
            cmd = {.precharge = OPEN, .positive = CLOSE, .negative = CLOSE};

            if (!FeedbackMatchesCommand(cmd, fb)) {
                new_state = ERR_RUNNING;
            }
            break;

        case LOW_SOC:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};
            break;

        case ERR_RUNNING:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};
            break;

        case SHUTDOWN:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};

            if (GetPrechargePercent() < threshold::PACK_SHUTDOWN_PERCENT) {
                new_state = IDLE;
            }
            break;
    }

    if (command == Command::OFF) {
        new_state = SHUTDOWN;
    }

    if (GetSocPercent() < threshold::LOW_VOLTAGE_PERCENT) {
        new_state = LOW_SOC;
    }

    contactor_command = cmd;

    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

void Update_100Hz(VehBus& veh_can, Command command, ContactorFeedbacks fb) {
    MeasureVoltages(veh_can);
    UpdateStateMachine(command, fb);
}

}  // namespace accumulator

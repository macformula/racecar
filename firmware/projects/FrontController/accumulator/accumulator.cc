#include "accumulator.hpp"

namespace accumulator {

static State state;
static ContactorCommands contactor_command;

static uint32_t elapsed;
static float pack_soc;

State GetState(void) {
    return state;
}

ContactorCommands GetContactorCommand(void) {
    return contactor_command;
}

void SetPackSoc(float _pack_soc) {
    pack_soc = _pack_soc;
}

void Init(void) {
    state = State::IDLE;
    contactor_command = {
        .precharge = ContactorCommand::OPEN,
        .positive = ContactorCommand::OPEN,
        .negative = ContactorCommand::OPEN,
    };
    elapsed = 0;
    pack_soc = 100;  // assume charged until told otherwise
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

void Update_100Hz(Command command, ContactorFeedbacks fb) {
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

            if (elapsed >= 10000) {
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
            break;

        case LOW_SOC:
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};
            break;

        case ERR_RUNNING:
            // The Simulink model never entered this state! Should remove but
            // Governor has behaviour which uses it. Uh oh!
            cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};
            break;
    }

    if (pack_soc < 30) {
        new_state = LOW_SOC;
    }

    if (command == Command::OFF) {
        cmd = {.precharge = OPEN, .positive = OPEN, .negative = OPEN};
        new_state = IDLE;
    }

    contactor_command = cmd;

    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

}  // namespace accumulator

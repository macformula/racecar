#include "motor_controller.hpp"

#include "bindings.hpp"
#include "generated/can/veh_messages.hpp"

namespace motor_controller {

using namespace generated::can;

static bool enabled = false;

bool sw = false;
static State state = State::OFF;
static uint32_t elapsed = 0;

void Init(void) {
    bindings::motor_ctrl_precharge_en.SetLow();
    bindings::motor_controller_en.SetLow();
    bindings::motor_ctrl_switch_en.SetLow();

    enabled = false;
    state = State::OFF;
    elapsed = 0;
}

State GetState(void) {
    return state;
}

void SetEnabled(bool enable) {
    enabled = enable;
}

static void StateMachine_100hz(void) {
    bool precharge = false;
    bool positive = false;

    State new_state = state;

    switch (state) {
        case State::OFF:
            precharge = false;
            positive = false;

            if (enabled) {
                new_state = State::PRECHARGING;
            }

            break;

        case State::PRECHARGING:
            precharge = true;
            positive = false;

            if (elapsed > 2000) {
                new_state = State::PRECHARGING_HANDOFF;
            }

            break;

        case State::PRECHARGING_HANDOFF:
            precharge = true;
            positive = true;

            if (elapsed > 50) {
                new_state = State::ON;
            }

            break;

        case State::ON:
            precharge = false;
            positive = true;
            break;
    }

    if (!enabled) {
        new_state = State::OFF;
    }

    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }

    bindings::motor_ctrl_precharge_en.Set(precharge);
    bindings::motor_controller_en.Set(positive);
}

void HandleSwitch(VehBus& veh_can) {
    auto msg = veh_can.GetRxInverterSwitchCommand();
    if (msg.has_value()) {
        sw = msg->CloseInverterSwitch();
    } else {
        sw = false;
    }

    bindings::motor_ctrl_switch_en.Set(sw);
}

bool GetSwitchClosed(void) {
    return sw;
}

void task_100hz(VehBus& veh_can) {
    StateMachine_100hz();
    HandleSwitch(veh_can);
}

}  // namespace motor_controller
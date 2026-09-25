#include "motor_controller.hpp"

#include "generated/can/veh_messages.hpp"

namespace macfe::lv {

using namespace generated::can;

State Motor_Controller::GetState(void) {
    return state;
}

void Motor_Controller::SetEnabled(bool enable) {
    enabled = enable;
}

void Motor_Controller::StateMachine_100hz(void) {
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

    _motor_ctrl_precharge_en.Set(precharge);
    _motor_controller_en.Set(positive);
}

void Motor_Controller::HandleSwitch(VehBus& veh_can) {
    auto msg = veh_can.GetRxInverterSwitchCommand();
    if (msg.has_value()) {
        sw = msg->CloseInverterSwitch();
    } else {
        sw = false;
    }

    _motor_ctrl_switch_en.Set(sw);
}

bool Motor_Controller::GetSwitchClosed(void) {
    return sw;
}

void Motor_Controller::task_100hz(VehBus& veh_can) {
    StateMachine_100hz();
    HandleSwitch(veh_can);
}

}  // namespace macfe::lv
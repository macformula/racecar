#pragma once

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "periph/gpio.hpp"

namespace motor_controller {

using State = generated::can::TxLvStatus::MotorControllerState_t;

struct motor_controller_periph {
    macfe::periph::DigitalOutput* motor_ctrl_precharge_en;
    macfe::periph::DigitalOutput* motor_controller_en;
    macfe::periph::DigitalOutput* motor_ctrl_switch_en;
};

class Controller {
public:
    Controller(motor_controller_periph motor_controller_periph)
        : periph(motor_controller_periph) {
        periph.motor_ctrl_precharge_en->SetLow();
        periph.motor_controller_en->SetLow();
        periph.motor_ctrl_switch_en->SetLow();

        enabled = false;
        state = State::OFF;
        elapsed = 0;
    };
    void SetEnabled(bool enable);
    State GetState(void);
    bool GetSwitchClosed(void);
    void task_100hz(generated::can::VehBus& veh_can);

private:
    void HandleSwitch(VehBus& veh_can);
    void StateMachine_100hz(void);
    motor_controller_periph periph;
    bool enabled = false;

    bool sw = false;
    State state = State::OFF;
    uint32_t elapsed = 0;
};
//! void Init(void);

}  // namespace motor_controller

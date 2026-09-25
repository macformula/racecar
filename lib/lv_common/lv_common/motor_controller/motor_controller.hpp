#pragma once

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "periph/gpio.hpp"

namespace macfe::lv {

using State = generated::can::TxLvStatus::Motor_ControllerState_t;

macfe::periph::DigitalOutput& motor_ctrl_precharge_en;
macfe::periph::DigitalOutput& motor_controller_en;
macfe::periph::DigitalOutput& motor_ctrl_switch_en;

class Motor_Controller {
public:
    Motor_Controller(macfe::periph::DigitalOutput& motor_ctrl_precharge_en,
                     macfe::periph::DigitalOutput& motor_controller_en,
                     macfe::periph::DigitalOutput& motor_ctrl_switch_en)
        : _motor_ctrl_precharge_en(motor_ctrl_precharge_en),
          _motor_controller_en(motor_controller_en),
          _motor_ctrl_switch_en(motor_ctrl_switch_en) {
        _motor_ctrl_precharge_en.SetLow();
        _motor_controller_en.SetLow();
        _motor_ctrl_switch_en.SetLow();

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
    macfe::periph::DigitalOutput& _motor_ctrl_precharge_en;
    macfe::periph::DigitalOutput& _motor_controller_en;
    macfe::periph::DigitalOutput& _motor_ctrl_switch_en;
    bool enabled = false;

    bool sw = false;
    State state = State::OFF;
    uint32_t elapsed = 0;
};

}  // namespace macfe::lv

#pragma once

#include "generated/can/veh_bus.hpp"

namespace motor_controller {

using State = generated::can::TxLvControllerStatus::MotorControllerState_t;

void Init(void);

void SetEnabled(bool enable);
State GetState(void);
bool GetSwitchClosed(void);

void task_100hz(generated::can::VehBus& veh_can);

}  // namespace motor_controller

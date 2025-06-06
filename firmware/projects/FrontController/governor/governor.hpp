#pragma once

#include "accumulator/accumulator.hpp"
#include "driver_interface/driver_interface.hpp"
#include "motors/motor_interface.hpp"

namespace governor {

using State = generated::can::TxFC_Status::GovStatus_t;
using DashState = generated::can::RxDashboardStatus::DashState_t;

accumulator::Command GetAccumulatorCmd(void);
motor::Command GetMotorCmd(void);
State GetState(void);

void Init(void);
void Update_100Hz(accumulator::State acc, motor::State mi, DashState dash);

}  // namespace governor

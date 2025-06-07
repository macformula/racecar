#pragma once

#include "accumulator/accumulator.hpp"
#include "motors/motors.hpp"

namespace governor {

using State = generated::can::TxFC_Status::GovStatus_t;
using DashState = generated::can::RxDashboardStatus::DashState_t;

accumulator::Command GetAccumulatorCmd(void);
motors::Command GetMotorCmd(void);
uint32_t GetElapsed(void);
State GetState(void);

void Init(void);
void Update_100Hz(accumulator::State acc, motors::State mi, DashState dash);

}  // namespace governor

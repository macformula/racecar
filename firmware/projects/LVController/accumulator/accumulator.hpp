#pragma once

#include "generated/can/veh_bus.hpp"

namespace accumulator {

void Init(void);

void SetEnabled(bool enable);

bool ConfirmContactorsOpen(void);
bool IsRunning(void);

void task_10hz(generated::can::VehBus& veh_can);

}  // namespace accumulator
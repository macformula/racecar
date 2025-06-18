#pragma once

namespace driver_interface {

float GetTorqueRequest(void);
bool IsBrakePressed(void);

void Update_100Hz(void);

}  // namespace driver_interface

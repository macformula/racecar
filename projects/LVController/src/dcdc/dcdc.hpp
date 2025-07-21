#pragma once

namespace dcdc {

void SetEnabled(bool enable);

float GetVoltage(void);
float GetAmps(void);

void task_100hz(void);

}  // namespace dcdc

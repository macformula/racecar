#pragma once

namespace fans {

void Init(void);

void SetPowerSetpoint(float power);
bool IsAtSetpoint(void);

void task_1khz(void);

}  // namespace fans

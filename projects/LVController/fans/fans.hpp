#pragma once

namespace fans {

void Init(void);

void SetPowerSetpoint(float power);
bool IsAtSetpoint(void);

void Update_100Hz(void);

}  // namespace fans

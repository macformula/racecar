/// @author Manush Patel
/// @date 2026-05-24

#pragma once

namespace dcdc {

void SetEnabled(bool enable);

float GetVoltage(void);
float GetAmps(void);
float MeasureLvBatteryVoltage(void);

void task_100hz(void);

}  // namespace dcdc

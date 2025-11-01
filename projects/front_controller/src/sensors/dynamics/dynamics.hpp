#pragma once

namespace sensors::dynamics {

float GetRightWheelRPM();
float GetLeftWheelRPM();
void Update_10Hz(void);

}  // namespace sensors::dynamics
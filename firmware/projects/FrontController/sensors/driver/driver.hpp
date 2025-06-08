#pragma once

#include "generated/can/veh_messages.hpp"

namespace sensors::driver {

// Full Left = -1
// Straight = 0
// Full Right = +1
float GetSteeringWheel(void);

float GetBrakePercent(void);
float GetAccelPercent1(void);
float GetAccelPercent2(void);

void Update_100Hz(void);

// Logging
generated::can::TxFC_apps_debug GetAppsDebugMsg(void);
generated::can::TxFC_bpps_steer_debug GetBppsSteerDebugMsg(void);

}  // namespace sensors::driver
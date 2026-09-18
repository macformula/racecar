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
generated::can::TxAppsDebug GetAppsDebugMsg(void);
generated::can::TxBppsSteerDebug GetBppsSteerDebugMsg(void);

}  // namespace sensors::driver
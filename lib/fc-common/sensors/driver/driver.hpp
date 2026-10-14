#pragma once

#include "generated/can/veh_messages.hpp"
#include "periph/analog_input.hpp"

namespace sensors::driver {

// Full Left = -1
// Straight = 0
// Full Right = +1
float GetSteeringWheel(void);

float GetBrakePercent(void);
float GetAccelPercent1(void);
float GetAccelPercent2(void);

void Update_100Hz(macfe::periph::AnalogInput& steering_angle_sensor,
                  macfe::periph::AnalogInput& brake_pressure_sensor,
                  macfe::periph::AnalogInput& accel_pedal_sensor1,
                  macfe::periph::AnalogInput& accel_pedal_sensor2);

// Logging
generated::can::TxAppsDebug GetAppsDebugMsg(void);
generated::can::TxBppsSteerDebug GetBppsSteerDebugMsg(void);

}  // namespace sensors::driver

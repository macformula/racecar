#pragma once

#include "generated/can/veh_messages.hpp"
#include "moving_average.hpp"
#include "periph/analog_input.hpp"

namespace sensors::driver {

class DriverSensors {
public:
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

private:
    void UpdateSteeringWheel(macfe::periph::AnalogInput& sensor);
    void UpdateBrake(macfe::periph::AnalogInput& sensor);
    void UpdatePedals(macfe::periph::AnalogInput& sensor1,
                      macfe::periph::AnalogInput& sensor2);
    float steer_v = 0;
    float brake_v = 0;
    float accel_v1 = 0;
    float accel_v2 = 0;
    macfe::MovingAverage<10> steering_wheel;
    macfe::MovingAverage<10> brake_percent;
    macfe::MovingAverage<10> accel_percent1;
    macfe::MovingAverage<10> accel_percent2;
};

}  // namespace sensors::driver

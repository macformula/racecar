#pragma once

#include "generated/can/veh_messages.hpp"
#include "moving_average.hpp"
#include "periph/analog_input.hpp"

namespace macfe::fc::sensors {

class DriverSensors {
public:
    DriverSensors(macfe::periph::AnalogInput& steering_angle_sensor,
                  macfe::periph::AnalogInput& brake_pressure_sensor,
                  macfe::periph::AnalogInput& accel_pedal_sensor1,
                  macfe::periph::AnalogInput& accel_pedal_sensor2)
        : _steering_angle_sensor(steering_angle_sensor),
          _brake_pressure_sensor(brake_pressure_sensor),
          _accel_pedal_sensor1(accel_pedal_sensor1),
          _accel_pedal_sensor2(accel_pedal_sensor2) {}

    float GetSteeringWheel(void);
    float GetBrakePercent(void);
    float GetAccelPercent1(void);
    float GetAccelPercent2(void);

    void Update_100Hz(void);

    // Logging
    generated::can::TxAppsDebug GetAppsDebugMsg(void);
    generated::can::TxBppsSteerDebug GetBppsSteerDebugMsg(void);

private:
    void UpdateSteeringWheel(void);
    void UpdateBrake(void);
    void UpdatePedals(void);

    macfe::periph::AnalogInput& _steering_angle_sensor;
    macfe::periph::AnalogInput& _brake_pressure_sensor;
    macfe::periph::AnalogInput& _accel_pedal_sensor1;
    macfe::periph::AnalogInput& _accel_pedal_sensor2;

    float steer_v = 0;
    float brake_v = 0;
    float accel_v1 = 0;
    float accel_v2 = 0;
    macfe::MovingAverage<10> steering_wheel;
    macfe::MovingAverage<10> brake_percent;
    macfe::MovingAverage<10> accel_percent1;
    macfe::MovingAverage<10> accel_percent2;
};

}  // namespace macfe::fc::sensors
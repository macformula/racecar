#include "driver.hpp"

#include "etl/algorithm.h"
#include "generated/can/veh_messages.hpp"
#include "moving_average.hpp"
#include "physical.hpp"

using namespace generated::can;
using macfe::MovingAverage;

namespace macfe::fc::sensors {

// Using moving average to smooth transients

float DriverSensors::GetSteeringWheel(void) {
    // return steering_wheel.GetValue(); no steering sensor yet
    return 0.f;
}

float DriverSensors::GetBrakePercent(void) {
    return brake_percent.GetValue();
}

float DriverSensors::GetAccelPercent1(void) {
    return accel_percent1.GetValue();
}

float DriverSensors::GetAccelPercent2(void) {
    return accel_percent2.GetValue();
}

// Linear interpolation / extrapolation
static inline float lerp(float t, float a, float b) {
    return (t - a) * (1 / (b - a));
}

void DriverSensors::UpdateSteeringWheel(
    macfe::periph::AnalogInput& steering_angle_sensor) {
    steer_v = steering_angle_sensor.ReadVoltage();
    float raw = lerp(steer_v, tuning::steer_volt_straight,
                     tuning::steer_volt_full_right);
    steering_wheel.LoadValue(etl::clamp<float>(raw, -1, 1));
}

void DriverSensors::UpdateBrake(
    macfe::periph::AnalogInput& brake_pressure_sensor) {
    brake_v = brake_pressure_sensor.ReadVoltage();
    float raw =
        100 * lerp(brake_v, tuning::bpps_volt_pos_0, tuning::bpps_volt_pos_100);
    brake_percent.LoadValue(etl::clamp<float>(raw, 0, 100));
}

void DriverSensors::UpdatePedals(
    macfe::periph::AnalogInput& accel_pedal_sensor1,
    macfe::periph::AnalogInput& accel_pedal_sensor2) {
    accel_v1 = accel_pedal_sensor1.ReadVoltage();
    float raw = 100. * lerp(accel_v1, tuning::apps1_volt_pos_0,
                            tuning::apps1_volt_pos_100);
    accel_percent1.LoadValue(etl::clamp<float>(raw, 0, 100));

    accel_v2 = accel_pedal_sensor2.ReadVoltage();
    raw = 100 *
          lerp(accel_v2, tuning::apps2_volt_pos_0, tuning::apps2_volt_pos_100);
    accel_percent2.LoadValue(etl::clamp<float>(raw, 0, 100));
}

void DriverSensors::Update_100Hz(
    macfe::periph::AnalogInput& steering_angle_sensor,
    macfe::periph::AnalogInput& brake_pressure_sensor,
    macfe::periph::AnalogInput& accel_pedal_sensor1,
    macfe::periph::AnalogInput& accel_pedal_sensor2) {
    UpdateSteeringWheel(steering_angle_sensor);
    UpdateBrake(brake_pressure_sensor);
    UpdatePedals(accel_pedal_sensor1, accel_pedal_sensor2);
}

// Logging

TxAppsDebug DriverSensors::GetAppsDebugMsg(void) {
    return TxAppsDebug{
        .apps1_raw_volt = accel_v1,
        .apps2_raw_volt = accel_v2,
        .apps1_percent = GetAccelPercent1(),
        .apps2_percent = GetAccelPercent2(),
    };
}
TxBppsSteerDebug DriverSensors::GetBppsSteerDebugMsg(void) {
    return TxBppsSteerDebug{
        .bpps_raw_volt = brake_v,
        .steer_raw_volt = steer_v,
        .bpps_percent = GetBrakePercent(),
        .steer_position = GetSteeringWheel(),
    };
}

}  // namespace macfe::fc::sensors

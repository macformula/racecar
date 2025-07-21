#include "driver.hpp"

#include "bindings.hpp"
#include "etl/algorithm.h"
#include "generated/can/veh_messages.hpp"
#include "moving_average.hpp"
#include "physical.hpp"

using namespace generated::can;
using macfe::MovingAverage;

namespace sensors::driver {

static float steer_v = 0;
static float brake_v = 0;
static float accel_v1 = 0;
static float accel_v2 = 0;

// Using moving average to smooth transients
static MovingAverage<10> steering_wheel;
static MovingAverage<10> brake_percent;
static MovingAverage<10> accel_percent1;
static MovingAverage<10> accel_percent2;

float GetSteeringWheel(void) {
    // return steering_wheel.GetValue(); no steering sensor yet
    return 0.f;
}

float GetBrakePercent(void) {
    return brake_percent.GetValue();
}

float GetAccelPercent1(void) {
    return accel_percent1.GetValue();
}

float GetAccelPercent2(void) {
    return accel_percent2.GetValue();
}

// Linear interpolation / extrapolation
static inline float lerp(float t, float a, float b) {
    return (t - a) * (1 / (b - a));
}

static void UpdateSteeringWheel(void) {
    steer_v = bindings::steering_angle_sensor.ReadVoltage();
    float raw = lerp(steer_v, tuning::steer_volt_straight,
                     tuning::steer_volt_full_right);
    steering_wheel.LoadValue(etl::clamp<float>(raw, -1, 1));
}

static void UpdateBrake(void) {
    brake_v = bindings::brake_pressure_sensor.ReadVoltage();
    float raw =
        100 * lerp(brake_v, tuning::bpps_volt_pos_0, tuning::bpps_volt_pos_100);
    brake_percent.LoadValue(etl::clamp<float>(raw, 0, 100));
}

static void UpdatePedals(void) {
    accel_v1 = bindings::accel_pedal_sensor1.ReadVoltage();
    float raw = 100. * lerp(accel_v1, tuning::apps1_volt_pos_0,
                            tuning::apps1_volt_pos_100);
    accel_percent1.LoadValue(etl::clamp<float>(raw, 0, 100));

    accel_v2 = bindings::accel_pedal_sensor2.ReadVoltage();
    raw = 100 *
          lerp(accel_v2, tuning::apps2_volt_pos_0, tuning::apps2_volt_pos_100);
    accel_percent2.LoadValue(etl::clamp<float>(raw, 0, 100));
}

void Update_100Hz(void) {
    UpdateSteeringWheel();
    UpdateBrake();
    UpdatePedals();
}

// Logging

TxAppsDebug GetAppsDebugMsg(void) {
    return TxAppsDebug{
        .apps1_raw_volt = accel_v1,
        .apps2_raw_volt = accel_v2,
        .apps1_percent = GetAccelPercent1(),
        .apps2_percent = GetAccelPercent2(),
    };
}
TxBppsSteerDebug GetBppsSteerDebugMsg(void) {
    return TxBppsSteerDebug{
        .bpps_raw_volt = brake_v,
        .steer_raw_volt = steer_v,
        .bpps_percent = GetBrakePercent(),
        .steer_position = GetSteeringWheel(),
    };
}

}  // namespace sensors::driver

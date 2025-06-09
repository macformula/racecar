#include "driver.hpp"

#include "bindings.hpp"
#include "etl/algorithm.h"
#include "generated/can/veh_messages.hpp"
#include "physical.hpp"

using namespace generated::can;

namespace sensors::driver {

static float steer_v = 0;
static float brake_v = 0;
static float accel_v1 = 0;
static float accel_v2 = 0;

static float steering_wheel = 0;
static float brake_percent = 0;
static float accel_percent1 = 0;
static float accel_percent2 = 0;

float GetSteeringWheel(void) {
    return steering_wheel;
}

float GetBrakePercent(void) {
    return brake_percent;
}

float GetAccelPercent1(void) {
    return accel_percent1;
}

float GetAccelPercent2(void) {
    return accel_percent2;
}

// Linear interpolation / extrapolation
static inline float lerp(float t, float a, float b) {
    return (t - a) * (1 / (b - a));
}

static void UpdateSteeringWheel(void) {
    steer_v = bindings::steering_angle_sensor.ReadVoltage();
    float raw = lerp(steer_v, tuning::steer_volt_straight,
                     tuning::steer_volt_full_right);
    steering_wheel = etl::clamp<float>(raw, -1, 1);
}

static void UpdateBrake(void) {
    brake_v = bindings::brake_pressure_sensor.ReadVoltage();
    float raw =
        100 * lerp(brake_v, tuning::bpps_volt_pos_0, tuning::bpps_volt_pos_100);
    brake_percent = etl::clamp<float>(raw, 0, 100);
}

static void UpdatePedals(void) {
    accel_v1 = bindings::accel_pedal_sensor1.ReadVoltage();
    float raw = 100. * lerp(accel_v1, tuning::apps1_volt_pos_0,
                            tuning::apps1_volt_pos_100);
    accel_percent1 = etl::clamp<float>(raw, 0, 100);

    accel_v2 = bindings::accel_pedal_sensor2.ReadVoltage();
    raw = 100 *
          lerp(accel_v2, tuning::apps2_volt_pos_0, tuning::apps2_volt_pos_100);
    accel_percent2 = etl::clamp<float>(raw, 0, 100);
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
        .apps1_percent = accel_percent1,
        .apps2_percent = accel_percent2,
    };
}
TxBppsSteerDebug GetBppsSteerDebugMsg(void) {
    return TxBppsSteerDebug{
        .bpps_raw_volt = brake_v,
        .steer_raw_volt = steer_v,
        .bpps_percent = brake_percent,
        .steer_position = steering_wheel,
    };
}

}  // namespace sensors::driver
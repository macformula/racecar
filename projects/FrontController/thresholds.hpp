#pragma once

#include <cstdint>

namespace threshold {

// accumulator
const float PRECHARGE_COMPLETE_PERCENT = 90.f;
const float LOW_VOLTAGE_PERCENT = 30.f;
const float PACK_SHUTDOWN_PERCENT = 10.f;
const float HV_MAX_VOLTAGE = 600.f;
const float HV_MIN_VOLTAGE = 250.f;

// driver
const float BRAKES_PRESSED_PERCENT = 10.f;
const float PEDAL_IMPLAUSIBLE_PERCENT = 10.f;
const float EV_4_7_APPS_ACTIVATE = 25.0f;   // rule EV.4.7
const float EV_4_7_APPS_DEACTIVATE = 5.0f;  // rule EV.4.7

// motor
const float MOTOR_TORQUE_LIMIT_PERCENT = 100.f;
const float MOTOR_SPEED_LIMIT_RPM = 5000.f;

}  // namespace threshold

namespace timeout {

using timeout_ms = uint32_t;

// accumulator
const timeout_ms PACK_VOLTAGE_UPDATE = 500;
const timeout_ms SHUTDOWN_RESET_DELAY = 5000;

const timeout_ms MIN_PRECHARGE_TIME = 10000;
const timeout_ms SPEAKER_DURATION = 2000;
const timeout_ms DASHBOARD_BOOT_TIME = 10000;

}  // namespace timeout

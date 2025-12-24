/// @author Aiden
/// @date 2025-12-20
/// @brief Calibration state machine for sensor calibration routines

#pragma once

#include <cstdint>

#include "generated/can/veh_bus.hpp"

namespace calibration {

enum class State {
    IDLE,                    // Not calibrating
    CALIBRATE_APPS_MIN,      // Find minimum APPS value (pedal released)
    CALIBRATE_APPS_MAX,      // Find maximum APPS value (pedal pressed)
    CALIBRATE_BPPS_MIN,      // Find minimum BPPS value (pedal released)
    CALIBRATE_BPPS_MAX,      // Find maximum BPPS value (pedal pressed)
    CALIBRATE_STEERING_CENTER,  // Find steering center position
    CALIBRATE_STEERING_RANGE,   // Find steering left/right limits
    CALIBRATE_IMU,           // IMU zero calibration (vehicle stationary)
    COMPLETE,                // Calibration finished successfully
    ERROR                    // Calibration error occurred
};

enum class Command {
    STOP,      // Disable calibration
    START      // Enable calibration
};

// ---------- Modifiers ----------
void SetCommand(Command cmd);

// ---------- Accessors ----------
State GetState(void);
bool IsCalibrating(void);

// ---------- Behavior ----------
void Init(void);
void Update_100Hz(generated::can::VehBus& veh_can);

}  // namespace calibration
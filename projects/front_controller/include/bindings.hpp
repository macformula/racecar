/// @author Blake Freer
/// @date 2024-05-29

#pragma once

#include "periph/analog_input.hpp"
#include "periph/can.hpp"
#include "periph/gpio.hpp"

namespace bindings {

// =========== CAN =========================================
extern macfe::periph::CanBase& veh_can_base;
extern macfe::periph::CanBase& pt_can_base;

// =========== Vehicle Dynamics Sensors ====================
extern macfe::periph::AnalogInput& suspension_travel1;
extern macfe::periph::AnalogInput& suspension_travel2;
extern macfe::periph::AnalogInput& wheel_speed_front_left;   // A
extern macfe::periph::AnalogInput& wheel_speed_front_right;  // A
extern macfe::periph::AnalogInput& wheel_speed_rear_left;    // B
extern macfe::periph::AnalogInput& wheel_speed_rear_right;   // B

// =========== Driver Control ==============================
extern macfe::periph::AnalogInput& steering_angle_sensor;
extern macfe::periph::AnalogInput& accel_pedal_sensor1;
extern macfe::periph::AnalogInput& accel_pedal_sensor2;
extern macfe::periph::AnalogInput& brake_pressure_sensor;

// =========== Status Monitors =============================
extern macfe::periph::AnalogInput& precharge_monitor;
extern macfe::periph::DigitalInput& bspd_fault;

// =========== Outputs =====================================
extern macfe::periph::DigitalOutput& dashboard_power_en;
extern macfe::periph::DigitalOutput& imd_fault_led_en;
extern macfe::periph::DigitalOutput& bms_fault_led_en;
extern macfe::periph::DigitalOutput& ready_to_drive_sig_en;
extern macfe::periph::DigitalOutput& debug_led;

extern void Initialize();

extern int GetTickMs();

extern void DelayMs(int ms);

extern void SoftwareReset();

extern int32_t GetLeftWheelTick();
extern int32_t GetRightWheelTick();

extern bool GetRightWheelForward();
extern bool GetLeftWheelForward();
}  // namespace bindings

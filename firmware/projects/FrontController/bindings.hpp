/// @author Blake Freer
/// @date 2024-05-29

#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

// =========== CAN =========================================
extern shared::periph::CanBase& veh_can_base;
extern shared::periph::CanBase& pt_can_base;

// =========== Vehicle Dynamics Sensors ====================
extern shared::periph::AnalogInput& suspension_travel1;
extern shared::periph::AnalogInput& suspension_travel2;
extern shared::periph::AnalogInput& wheel_speed_front_left;   // A
extern shared::periph::AnalogInput& wheel_speed_front_right;  // A
extern shared::periph::AnalogInput& wheel_speed_rear_left;    // B
extern shared::periph::AnalogInput& wheel_speed_rear_right;   // B

// =========== Driver Control ==============================
extern shared::periph::AnalogInput& steering_angle_sensor;
extern shared::periph::AnalogInput& accel_pedal_sensor1;
extern shared::periph::AnalogInput& accel_pedal_sensor2;
extern shared::periph::AnalogInput& brake_pressure_sensor;

// =========== Status Monitors =============================
extern shared::periph::AnalogInput& precharge_monitor;
extern shared::periph::DigitalInput& bspd_fault;

// =========== Outputs =====================================
extern shared::periph::DigitalOutput& dashboard_power_en;
extern shared::periph::DigitalOutput& imd_fault_led_en;
extern shared::periph::DigitalOutput& bms_fault_led_en;
extern shared::periph::DigitalOutput& ready_to_drive_sig_en;
extern shared::periph::DigitalOutput& debug_led;

extern void Initialize();

extern int GetTickMs();

extern void DelayMs(int ms);

extern void SoftwareReset();

}  // namespace bindings

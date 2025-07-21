/// @author Blake Freer
/// @date 2024-04-24

#pragma once

#include <cstdint>

#include "periph/analog_input.hpp"
#include "periph/can.hpp"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

namespace bindings {

// Tractive System Status Indicator (2025 Rule EV.5.11.5)
extern macfe::periph::DigitalOutput& tssi_en;
extern macfe::periph::DigitalOutput& tssi_red_signal;
extern macfe::periph::DigitalOutput& tssi_green_signal;
extern macfe::periph::DigitalInput& imd_fault;
extern macfe::periph::DigitalInput& bms_fault;

// Powertrain Cooling
extern macfe::periph::DigitalOutput& powertrain_pump1_en;
extern macfe::periph::DigitalOutput& powertrain_pump2_en;
extern macfe::periph::DigitalOutput& powertrain_fan1_en;
extern macfe::periph::DigitalOutput& powertrain_fan2_en;
extern macfe::periph::PWMOutput& powertrain_fan_pwm;  // macfe by both fans

// Motor Controller (i.e. Inverters)
extern macfe::periph::DigitalOutput& motor_controller_en;
extern macfe::periph::DigitalOutput& motor_ctrl_precharge_en;
extern macfe::periph::DigitalOutput& motor_ctrl_switch_en;

// Subsystems
extern macfe::periph::DigitalOutput& accumulator_en;
extern macfe::periph::DigitalOutput& front_controller_en;
extern macfe::periph::DigitalOutput& imu_gps_en;
extern macfe::periph::DigitalOutput& shutdown_circuit_en;

// DCDC System & Measurement
extern macfe::periph::DigitalOutput& dcdc_en;
extern macfe::periph::DigitalOutput& dcdc_sense_select;
extern macfe::periph::AnalogInput& dcdc_sense;

// Other IO
extern macfe::periph::DigitalOutput& brake_light_en;
extern macfe::periph::AnalogInput& suspension_travel3;
extern macfe::periph::AnalogInput& suspension_travel4;
extern macfe::periph::CanBase& veh_can_base;

extern void Initialize();

extern void DelayMS(uint32_t ms);

extern int GetTick();

extern void SoftwareReset();

}  // namespace bindings
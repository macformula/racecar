/// @author Blake Freer
/// @date 2024-04-24

#pragma once

#include <cstdint>

#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

namespace bindings {

// Tractive System Status Indicator (2025 Rule EV.5.11.5)
extern shared::periph::DigitalOutput& tssi_en;
extern shared::periph::DigitalOutput& tssi_red_signal;
extern shared::periph::DigitalOutput& tssi_green_signal;
extern shared::periph::DigitalInput& imd_fault;
extern shared::periph::DigitalInput& bms_fault;

// Powertrain Cooling
extern shared::periph::DigitalOutput& powertrain_pump1_en;
extern shared::periph::DigitalOutput& powertrain_pump2_en;
extern shared::periph::DigitalOutput& powertrain_fan1_en;
extern shared::periph::DigitalOutput& powertrain_fan2_en;
extern shared::periph::PWMOutput& powertrain_fan_pwm;  // shared by both fans

// Motor Controller (i.e. Inverters)
extern shared::periph::DigitalOutput& motor_controller_en;
extern shared::periph::DigitalOutput& motor_ctrl_precharge_en;
extern shared::periph::DigitalOutput& motor_ctrl_switch_en;

// Subsystems
extern shared::periph::DigitalOutput& accumulator_en;
extern shared::periph::DigitalOutput& front_controller_en;
extern shared::periph::DigitalOutput& imu_gps_en;
extern shared::periph::DigitalOutput& raspberry_pi_en;
extern shared::periph::DigitalOutput& shutdown_circuit_en;

// DCDC System & Measurement
extern shared::periph::DigitalOutput& dcdc_en;
extern shared::periph::DigitalOutput& dcdc_sense_select;
extern shared::periph::AnalogInput& dcdc_sense;

// Other IO
extern shared::periph::DigitalOutput& brake_light_en;
extern shared::periph::AnalogInput& suspension_travel3;
extern shared::periph::AnalogInput& suspension_travel4;
extern shared::periph::CanBase& veh_can_base;

extern void Initialize();

extern void DelayMS(uint32_t ms);

extern int GetTick();

extern void SoftwareReset();

}  // namespace bindings
/// @author Blake Freer
/// @date 2024-04-24

#pragma once

#include <cstdint>

#include "shared/periph/can.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"

namespace bindings {

extern shared::periph::DigitalOutput& tsal_en;
extern shared::periph::DigitalOutput& raspberry_pi_en;
extern shared::periph::DigitalOutput& front_controller_en;
extern shared::periph::DigitalOutput& speedgoat_en;
extern shared::periph::DigitalOutput& accumulator_en;
extern shared::periph::DigitalOutput& motor_ctrl_precharge_en;
extern shared::periph::DigitalOutput& motor_ctrl_en;
extern shared::periph::DigitalOutput& imu_gps_en;
extern shared::periph::DigitalOutput& shutdown_circuit_en;
extern shared::periph::DigitalOutput& inverter_switch_en;

extern shared::periph::DigitalOutput& dcdc_en;
extern shared::periph::DigitalInput& dcdc_valid;
extern shared::periph::DigitalOutput& dcdc_led_en;
extern shared::periph::DigitalOutput& powertrain_pump_en;
extern shared::periph::DigitalOutput& powertrain_fan_en;
extern shared::periph::PWMOutput& powertrain_fan_pwm;

extern shared::periph::CanBase& veh_can_base;

extern void Initialize();

extern void DelayMS(uint32_t ms);

}  // namespace bindings
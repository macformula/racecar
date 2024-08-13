/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "mcal/windows/periph/can.h"
#include "mcal/windows/periph/gpio.h"
#include "mcal/windows/periph/pwm.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/identity.h"

// clang-format off
namespace mcal {
using namespace mcal::windows::periph;
DigitalOutput tsal_en = DigitalOutput{"TSAL Enable"};
DigitalOutput raspberry_pi_en = DigitalOutput{"RASPI Enable"};
DigitalOutput front_controller_en = DigitalOutput{"FRONT CONTROLLER Enable"};
DigitalOutput speedgoat_en = DigitalOutput{"SPEEDGOAT Enable"};
DigitalOutput accumulator_en = DigitalOutput{"ACCUMULATOR Enable"};
DigitalOutput motor_ctrl_precharge_en = DigitalOutput{"MOTOR CTRL LV PRECHARGE Enable"};
DigitalOutput motor_ctrl_en = DigitalOutput{"MOTOR CTRL LV Enable"};
DigitalOutput imu_gps_en = DigitalOutput{"IMU GPS Enable"};
DigitalOutput shutdown_circuit_en = DigitalOutput{"SHUTDOWN CIRCUIT Enable"};
DigitalOutput inverter_switch_en = DigitalOutput{"INVERTER SWITCH Enable"};

DigitalOutput dcdc_en = DigitalOutput{"DCDC Enable"};
DigitalInput dcdc_valid = DigitalInput{"DCDC Valid"};
DigitalOutput dcdc_led_en = DigitalOutput{"DCDC LED Enable"};
DigitalOutput powertrain_fan_en = DigitalOutput{"POWERTRAIN FAN Enable"};
DigitalOutput powertrain_pump_en = DigitalOutput{"POWERTRAIN PUMP Enable"};
PWMOutput powertrain_fan_pwm = PWMOutput{"POWERTRAIN FAN PWM"};

CanBase veh_can_base = CanBase{"can0"};
}  // namespace mcal


namespace bindings {
shared::periph::DigitalOutput& tsal_en = mcal::tsal_en;
shared::periph::DigitalOutput& raspberry_pi_en = mcal::raspberry_pi_en;
shared::periph::DigitalOutput& front_controller_en = mcal::front_controller_en;
shared::periph::DigitalOutput& speedgoat_en = mcal::speedgoat_en;
shared::periph::DigitalOutput& accumulator_en = mcal::accumulator_en;
shared::periph::DigitalOutput& motor_ctrl_precharge_en = mcal::motor_ctrl_precharge_en;
shared::periph::DigitalOutput& motor_ctrl_en = mcal::motor_ctrl_en;
shared::periph::DigitalOutput& imu_gps_en = mcal::imu_gps_en;
shared::periph::DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;
shared::periph::DigitalOutput& inverter_switch_en = mcal::inverter_switch_en;

shared::periph::DigitalOutput& dcdc_en = mcal::dcdc_en;
shared::periph::DigitalInput& dcdc_valid = mcal::dcdc_valid;
shared::periph::DigitalOutput& dcdc_led_en = mcal::dcdc_led_en;
shared::periph::DigitalOutput& powertrain_fan_en = mcal::powertrain_fan_en;
shared::periph::DigitalOutput& powertrain_pump_en = mcal::powertrain_pump_en;
shared::periph::PWMOutput& powertrain_fan_pwm = mcal::powertrain_fan_pwm;

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
// clang-format on

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
    mcal::veh_can_base.Setup();
}

void DelayMS(uint32_t milliseconds) {
    std::cout << "[Delaying for " << milliseconds << " milliseconds]"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
}  // namespace bindings
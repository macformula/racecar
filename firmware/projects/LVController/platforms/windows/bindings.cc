/// @author Blake Freer
/// @date 2023-12-25

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "mcal/windows/periph/gpio.h"
#include "mcal/windows/periph/pwm.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/identity.h"

namespace mcal {
using namespace windows::periph;

DigitalOutput tsal_on_enable_en{"TSAL Enable"};
DigitalOutput raspberry_pi_en{"RASPI Enable"};
DigitalOutput front_controller_en{"FRONT CONTROLLER Enable"};
DigitalOutput speedgoat_en{"SPEEDGOAT Enable"};
DigitalOutput accumulator_en{"ACCUMULATOR Enable"};
DigitalOutput motor_ctrl_precharge_en{"MOTOR CTRL LV PRECHARGE Enable"};
DigitalOutput motor_ctrl_en{"MOTOR CTRL LV Enable"};
DigitalOutput imu_gps_en{"IMU GPS Enable"};

DigitalOutput shutdown_circuit_en{"SHUTDOWN CIRCUIT Enable"};
DigitalOutput dcdc_en{"DCDC Enable"};
DigitalInput dcdc_valid{"DCDC Valid"};
DigitalOutput dcdc_led_en{"DCDC LED Enable"};
DigitalOutput powertrain_fan_en{"POWERTRAIN FAN Enable"};
DigitalOutput powertrain_pump_en{"POWERTRAIN PUMP Enable"};
PWMOutput powertrain_fan_pwm{"POWERTRAIN FAN PWM"};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalOutput& tsal_en = mcal::tsal_on_enable_en;
shared::periph::DigitalOutput& raspberry_pi_en = mcal::raspberry_pi_en;
shared::periph::DigitalOutput& front_controller_en = mcal::front_controller_en;
shared::periph::DigitalOutput& speedgoat_en = mcal::speedgoat_en;
shared::periph::DigitalOutput& accumulator_en = mcal::accumulator_en;
shared::periph::DigitalOutput& motor_ctrl_precharge_en =
    mcal::motor_ctrl_precharge_en;
shared::periph::DigitalOutput& motor_ctrl_en = mcal::motor_ctrl_en;
shared::periph::DigitalOutput& imu_gps_en = mcal::imu_gps_en;
shared::periph::DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

shared::periph::DigitalOutput& dcdc_en = mcal::dcdc_en;
shared::periph::DigitalInput& dcdc_valid = mcal::dcdc_valid;
shared::periph::DigitalOutput& dcdc_led_en = mcal::dcdc_led_en;

shared::periph::DigitalOutput& powertrain_fan_en = mcal::powertrain_fan_en;
shared::periph::DigitalOutput& powertrain_pump_en = mcal::powertrain_pump_en;
shared::periph::PWMOutput& powertrain_fan_pwm = mcal::powertrain_fan_pwm;

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

}  // namespace bindings

void DelayMS(uint32_t milliseconds) {
    std::cout << "[Delaying for " << milliseconds << " milliseconds]"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
/// @author Blake Freer
/// @date 2023-12-25
#include "bindings.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "bindings.h"
#include "mcal/windows/periph/gpio.h"
#include "mcal/windows/periph/pwm.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/identity.h"

namespace bindings {
using namespace mcal::windows::periph;

// clang-format off
shared::periph::DigitalOutput&& tsal_en = DigitalOutput{"TSAL Enable"};
shared::periph::DigitalOutput&& raspberry_pi_en = DigitalOutput{"RASPI Enable"};
shared::periph::DigitalOutput&& front_controller_en = DigitalOutput{"FRONT CONTROLLER Enable"};
shared::periph::DigitalOutput&& speedgoat_en = DigitalOutput{"SPEEDGOAT Enable"};
shared::periph::DigitalOutput&& accumulator_en = DigitalOutput{"ACCUMULATOR Enable"};
shared::periph::DigitalOutput&& motor_ctrl_precharge_en = DigitalOutput{"MOTOR CTRL LV PRECHARGE Enable"};
shared::periph::DigitalOutput&& motor_ctrl_en = DigitalOutput{"MOTOR CTRL LV Enable"};
shared::periph::DigitalOutput&& imu_gps_en = DigitalOutput{"IMU GPS Enable"};
shared::periph::DigitalOutput&& shutdown_circuit_en = DigitalOutput{"SHUTDOWN CIRCUIT Enable"};

shared::periph::DigitalOutput&& dcdc_en = DigitalOutput{"DCDC Enable"};
shared::periph::DigitalInput&& dcdc_valid = DigitalInput{"DCDC Valid"};
shared::periph::DigitalOutput&& dcdc_led_en = DigitalOutput{"DCDC LED Enable"};
shared::periph::DigitalOutput&& powertrain_fan_en = DigitalOutput{"POWERTRAIN FAN Enable"};
shared::periph::DigitalOutput&& powertrain_pump_en = DigitalOutput{"POWERTRAIN PUMP Enable"};
shared::periph::PWMOutput&& powertrain_fan_pwm = PWMOutput{"POWERTRAIN FAN PWM"};
// clang-format on

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

void DelayMS(uint32_t milliseconds) {
    std::cout << "[Delaying for " << milliseconds << " milliseconds]"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
}  // namespace bindings
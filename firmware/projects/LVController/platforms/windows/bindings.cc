/// @author Blake Freer
/// @date 2023-12-25

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "mcal/windows/periph/gpio.h"
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

shared::util::IdentityMap<float> powertrain_fan_power_to_duty{};

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

shared::util::Mapper<float>& powertrain_fan_power_to_duty =
    mcal::powertrain_fan_power_to_duty;

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

}  // namespace bindings

void DelayMS(uint32_t milliseconds) {
    std::cout << "[Delaying for " << milliseconds << " milliseconds]"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
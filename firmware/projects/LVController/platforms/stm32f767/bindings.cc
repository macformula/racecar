/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
// fw includes

#include <chrono>

#include "gpio.h"
#include "inc/app.h"
#include "main.h"
#include "mcal/stm32f767/periph/gpio.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/identity.h"
#include "shared/util/mappers/mapper.h"
#include "stm32f7xx_hal.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f767::periph;

DigitalOutput tsal_on_enable_en{TSAL_EN_GPIO_Port, TSAL_EN_Pin};
DigitalOutput raspberry_pi_en{RASPI_EN_GPIO_Port, RASPI_EN_Pin};
DigitalOutput front_controller_en{FRONT_CONTROLLER_EN_GPIO_Port,
                                  FRONT_CONTROLLER_EN_Pin};
DigitalOutput speedgoat_en{SPEEDGOAT_EN_GPIO_Port, SPEEDGOAT_EN_Pin};
DigitalOutput accumulator_en{ACCUMULATOR_EN_GPIO_Port, ACCUMULATOR_EN_Pin};
DigitalOutput motor_ctrl_precharge_en{MOTOR_CONTROLLER_PRECHARGE_EN_GPIO_Port,
                                      MOTOR_CONTROLLER_PRECHARGE_EN_Pin};
DigitalOutput motor_ctrl_en{MOTOR_CONTROLLER_EN_GPIO_Port,
                            MOTOR_CONTROLLER_EN_Pin};
DigitalOutput imu_gps_en{IMU_GPS_EN_GPIO_Port, IMU_GPS_EN_Pin};

DigitalOutput dcdc_en DigitalInput dcdc_valid{MUX_DCDC_VALID_GPIO_Port,
                                              MUX_DCDC_VALID_Pin};

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

shared::periph::DigitalOutput& dcdc_en = mcal::dcdc_en;
Status dcdc_valid{mcal::dcdc_valid, true};
shared::util::Mapper<float>& powertrain_fan_power_to_duty =
    mcal::powertrain_fan_power_to_duty;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings

void DelayMS(uint32_t milliseconds) {
    HAL_Delay(milliseconds);
}
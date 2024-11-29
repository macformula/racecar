/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
// fw includes

// CubeMX
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/can.hpp"
#include "shared/periph/can.hpp"
#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"

// Firmware
#include "../../bindings.hpp"
#include "mcal/stm32f767/periph/gpio.hpp"
#include "mcal/stm32f767/periph/pwm.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/identity.hpp"
#include "shared/util/mappers/mapper.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace mcal::stm32f767::periph;
CanBase veh_can_base{&hcan3};

}  // namespace mcal

namespace mcal {
using namespace stm32f767::periph;
DigitalOutput tsal_en{
    TSAL_EN_GPIO_Port,
    TSAL_EN_Pin,
};
DigitalOutput raspberry_pi_en{
    RASPI_EN_GPIO_Port,
    RASPI_EN_Pin,
};
DigitalOutput front_controller_en{
    FRONT_CONTROLLER_EN_GPIO_Port,
    FRONT_CONTROLLER_EN_Pin,
};
DigitalOutput speedgoat_en{
    SPEEDGOAT_EN_GPIO_Port,
    SPEEDGOAT_EN_Pin,
};
DigitalOutput accumulator_en{
    ACCUMULATOR_EN_GPIO_Port,
    ACCUMULATOR_EN_Pin,
};
DigitalOutput motor_ctrl_precharge_en{
    MOTOR_CONTROLLER_PRECHARGE_EN_GPIO_Port,
    MOTOR_CONTROLLER_PRECHARGE_EN_Pin,
};
DigitalOutput motor_ctrl_en{
    MOTOR_CONTROLLER_EN_GPIO_Port,
    MOTOR_CONTROLLER_EN_Pin,
};
DigitalOutput imu_gps_en{
    IMU_GPS_EN_GPIO_Port,
    IMU_GPS_EN_Pin,
};
DigitalOutput shutdown_circuit_en{
    SHUTDOWN_CIRCUIT_EN_GPIO_Port,
    SHUTDOWN_CIRCUIT_EN_Pin,
};
DigitalOutput inverter_switch_en{
    INVERTER_EN_GPIO_Port,
    INVERTER_EN_Pin,
};
DigitalOutput dcdc_en{
    DCDC_EN_GPIO_Port,
    DCDC_EN_Pin,
};
DigitalInput dcdc_valid{
    MUX_DCDC_VALID_GPIO_Port,
    MUX_DCDC_VALID_Pin,
};
DigitalOutput dcdc_led_en{
    DCDC_ON_LED_EN_GPIO_Port,
    DCDC_ON_LED_EN_Pin,
};
DigitalOutput powertrain_fan_en{
    POWERTRAIN_FAN_EN_GPIO_Port,
    POWERTRAIN_FAN_EN_Pin,
};
DigitalOutput powertrain_pump_en{
    POWERTRAIN_PUMP_EN_GPIO_Port,
    POWERTRAIN_PUMP_EN_Pin,
};
PWMOutput powertrain_fan_pwm{
    &htim2,
    HAL_TIM_ACTIVE_CHANNEL_1,
};
}  // namespace mcal

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

shared::periph::DigitalOutput& tsal_en = mcal::tsal_en;
shared::periph::DigitalOutput& raspberry_pi_en = mcal::raspberry_pi_en;
shared::periph::DigitalOutput& front_controller_en = mcal::front_controller_en;
shared::periph::DigitalOutput& speedgoat_en = mcal::speedgoat_en;
shared::periph::DigitalOutput& accumulator_en = mcal::accumulator_en;
shared::periph::DigitalOutput& motor_ctrl_precharge_en =
    mcal::motor_ctrl_precharge_en;
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

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN3_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();

    mcal::veh_can_base.Setup();
}

void DelayMS(uint32_t milliseconds) {
    HAL_Delay(milliseconds);
}

}  // namespace bindings
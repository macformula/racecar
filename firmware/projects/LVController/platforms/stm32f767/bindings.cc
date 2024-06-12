/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
// fw includes

// CubeMX
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/can.h"
#include "shared/periph/can.h"
#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"

// Firmware
#include "mcal/stm32f767/periph/gpio.h"
#include "mcal/stm32f767/periph/pwm.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/identity.h"
#include "shared/util/mappers/mapper.h"

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

namespace bindings {
using namespace mcal::stm32f767::periph;

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

shared::periph::DigitalOutput&& tsal_en = DigitalOutput{
    TSAL_EN_GPIO_Port,
    TSAL_EN_Pin,
};
shared::periph::DigitalOutput&& raspberry_pi_en = DigitalOutput{
    RASPI_EN_GPIO_Port,
    RASPI_EN_Pin,
};
shared::periph::DigitalOutput&& front_controller_en = DigitalOutput{
    FRONT_CONTROLLER_EN_GPIO_Port,
    FRONT_CONTROLLER_EN_Pin,
};
shared::periph::DigitalOutput&& speedgoat_en = DigitalOutput{
    SPEEDGOAT_EN_GPIO_Port,
    SPEEDGOAT_EN_Pin,
};
shared::periph::DigitalOutput&& accumulator_en = DigitalOutput{
    ACCUMULATOR_EN_GPIO_Port,
    ACCUMULATOR_EN_Pin,
};
shared::periph::DigitalOutput&& motor_ctrl_precharge_en = DigitalOutput{
    MOTOR_CONTROLLER_PRECHARGE_EN_GPIO_Port,
    MOTOR_CONTROLLER_PRECHARGE_EN_Pin,
};
shared::periph::DigitalOutput&& motor_ctrl_en = DigitalOutput{
    MOTOR_CONTROLLER_EN_GPIO_Port,
    MOTOR_CONTROLLER_EN_Pin,
};
shared::periph::DigitalOutput&& imu_gps_en = DigitalOutput{
    IMU_GPS_EN_GPIO_Port,
    IMU_GPS_EN_Pin,
};
shared::periph::DigitalOutput&& shutdown_circuit_en = DigitalOutput{
    SHUTDOWN_CIRCUIT_EN_GPIO_Port,
    SHUTDOWN_CIRCUIT_EN_Pin,
};
shared::periph::DigitalOutput&& inverter_switch_en = DigitalOutput{
    INVERTER_SWITCH_EN_GPIO_Port,
    INVERTER_SWITCH_EN_Pin,
};
shared::periph::DigitalOutput&& dcdc_en = DigitalOutput{
    DCDC_EN_GPIO_Port,
    DCDC_EN_Pin,
};
shared::periph::DigitalInput&& dcdc_valid = DigitalInput{
    MUX_DCDC_VALID_GPIO_Port,
    MUX_DCDC_VALID_Pin,
};
shared::periph::DigitalOutput&& dcdc_led_en = DigitalOutput{
    DCDC_ON_LED_EN_GPIO_Port,
    DCDC_ON_LED_EN_Pin,
};
shared::periph::DigitalOutput&& powertrain_fan_en = DigitalOutput{
    POWERTRAIN_FAN_EN_GPIO_Port,
    POWERTRAIN_FAN_EN_Pin,
};
DigitalOutput powertrain_pump_en{
    POWERTRAIN_PUMP_EN_GPIO_Port,
    POWERTRAIN_PUMP_EN_Pin,
};
shared::periph::PWMOutput&& powertrain_fan_pwm = PWMOutput{
    &htim2,
    HAL_TIM_ACTIVE_CHANNEL_1,
};

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN3_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
}

void DelayMS(uint32_t milliseconds) {
    HAL_Delay(milliseconds);
}

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan3) {
        mcal::veh_can_base.AddRxMessageToQueue();
    }
}
}
}  // namespace bindings
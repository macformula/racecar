/// @author Blake Freer
/// @date 2023-12-25

#include <stdio.h>

#include <string>

// cubemx files
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/can.h"
#include "shared/comms/can/can_msg.h"
#include "tim.h"

// fw imports
#include "../../bindings.h"
#include "mcal/stm32f767/periph/adc.h"
#include "mcal/stm32f767/periph/gpio.h"
#include "mcal/stm32f767/periph/pwm.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined in
 * cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f767::periph;

ADCInput temp_sensor_adc_1{&hadc1, SENS_1_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_2{&hadc1, SENS_2_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_3{&hadc1, SENS_3_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_4{&hadc1, SENS_4_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_5{&hadc1, SENS_5_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_6{&hadc1, SENS_6_UC_IN_CHANNEL};

PWMOutput fan_controller_pwm{&htim4, TIM_CHANNEL_1};
DigitalOutput debug_led_green{DEBUG_LED1_GPIO_Port, DEBUG_LED1_Pin};
DigitalOutput debug_led_red{DEBUG_LED2_GPIO_Port, DEBUG_LED2_Pin};
DigitalOutput debug_led_nucleo{NUCLEO_RED_LED_GPIO_Port, NUCLEO_RED_LED_Pin};

CanBase veh_can_base{&hcan2};

}  // namespace mcal

namespace bindings {
shared::periph::ADCInput& temp_sensor_adc_1 = mcal::temp_sensor_adc_1;
shared::periph::ADCInput& temp_sensor_adc_2 = mcal::temp_sensor_adc_2;
shared::periph::ADCInput& temp_sensor_adc_3 = mcal::temp_sensor_adc_3;
shared::periph::ADCInput& temp_sensor_adc_4 = mcal::temp_sensor_adc_4;
shared::periph::ADCInput& temp_sensor_adc_5 = mcal::temp_sensor_adc_5;
shared::periph::ADCInput& temp_sensor_adc_6 = mcal::temp_sensor_adc_6;

shared::periph::PWMOutput& fan_controller_pwm = mcal::fan_controller_pwm;
shared::periph::DigitalOutput& debug_led_green = mcal::debug_led_green;
shared::periph::DigitalOutput& debug_led_red = mcal::debug_led_red;
// shared::periph::DigitalOutput& debug_led_red = mcal::debug_led_nucleo;

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    HAL_Init();
    SystemClock_Config();
    MX_ADC1_Init();
    MX_TIM4_Init();
    MX_GPIO_Init();
    MX_CAN2_Init();

    mcal::veh_can_base.Setup();
}

}  // namespace bindings

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    // TMS doesn't care about any Rx messages but we need to call this to
    // clear the interrupt flag.
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
}
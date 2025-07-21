/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// fw imports
#include "bindings.hpp"
#include "mcal/stm32f/analog_input.hpp"
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "mcal/stm32f/pwm.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined in
 * cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f;

AnalogInput temp_sensor_adc_1{&hadc1, SENS_1_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_2{&hadc1, SENS_2_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_3{&hadc1, SENS_3_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_4{&hadc1, SENS_4_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_5{&hadc1, SENS_5_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_6{&hadc1, SENS_6_UC_IN_CHANNEL};

PWMOutput fan_controller_pwm{&htim4, TIM_CHANNEL_1};
DigitalOutput debug_led_green{DEBUG_LED1_GPIO_Port, DEBUG_LED1_Pin};
DigitalOutput debug_led_red{DEBUG_LED2_GPIO_Port, DEBUG_LED2_Pin};
DigitalOutput debug_led_nucleo{NUCLEO_RED_LED_GPIO_Port, NUCLEO_RED_LED_Pin};

CanBase veh_can_base{&hcan2};

}  // namespace mcal

namespace bindings {
macfe::periph::AnalogInput& temp_sensor_adc_1 = mcal::temp_sensor_adc_1;
macfe::periph::AnalogInput& temp_sensor_adc_2 = mcal::temp_sensor_adc_2;
macfe::periph::AnalogInput& temp_sensor_adc_3 = mcal::temp_sensor_adc_3;
macfe::periph::AnalogInput& temp_sensor_adc_4 = mcal::temp_sensor_adc_4;
macfe::periph::AnalogInput& temp_sensor_adc_5 = mcal::temp_sensor_adc_5;
macfe::periph::AnalogInput& temp_sensor_adc_6 = mcal::temp_sensor_adc_6;

macfe::periph::PWMOutput& fan_controller_pwm = mcal::fan_controller_pwm;
macfe::periph::DigitalOutput& debug_led_green = mcal::debug_led_green;
macfe::periph::DigitalOutput& debug_led_red = mcal::debug_led_red;
// macfe::periph::DigitalOutput& debug_led_red = mcal::debug_led_nucleo;

macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    HAL_Init();
    SystemClock_Config();
    MX_ADC1_Init();
    MX_TIM4_Init();
    MX_GPIO_Init();
    MX_CAN2_Init();

    mcal::veh_can_base.Setup();
}

uint32_t GetCurrentTimeMs() {
    return HAL_GetTick();
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

}  // namespace bindings
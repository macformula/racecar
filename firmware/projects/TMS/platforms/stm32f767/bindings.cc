/// @author Blake Freer
/// @date 2023-12-25

#include <string>

// cubemx files
#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// fw imports
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

namespace bindings {
const shared::periph::ADCInput& temp_sensor_adc_1 =
    mcal::periph::ADCInput{&hadc1, SENS_1_UC_IN_CHANNEL};

const shared::periph::ADCInput& temp_sensor_adc_2 =
    mcal::periph::ADCInput{&hadc1, SENS_2_UC_IN_CHANNEL};

const shared::periph::ADCInput& temp_sensor_adc_3 =
    mcal::periph::ADCInput{&hadc1, SENS_3_UC_IN_CHANNEL};

const shared::periph::ADCInput& temp_sensor_adc_4 =
    mcal::periph::ADCInput{&hadc1, SENS_4_UC_IN_CHANNEL};

const shared::periph::ADCInput& temp_sensor_adc_5 =
    mcal::periph::ADCInput{&hadc1, SENS_5_UC_IN_CHANNEL};

const shared::periph::ADCInput& temp_sensor_adc_6 =
    mcal::periph::ADCInput{&hadc1, SENS_6_UC_IN_CHANNEL};

const shared::periph::PWMOutput& fan_controller_pwm =
    mcal::periph::PWMOutput{&htim4, TIM_CHANNEL_1};

const shared::periph::DigitalOutput& debug_do_green =
    mcal::periph::DigitalOutput{DEBUG_LED_GREEN_GPIO_Port, DEBUG_LED_GREEN_Pin};

const shared::periph::DigitalOutput& debug_do_red =
    mcal::periph::DigitalOutput{DEBUG_LED_RED_GPIO_Port, DEBUG_LED_RED_Pin};

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_ADC1_Init();
    MX_TIM4_Init();
    MX_GPIO_Init();
}

void Log(std::string s) {
    return;
}

}  // namespace bindings

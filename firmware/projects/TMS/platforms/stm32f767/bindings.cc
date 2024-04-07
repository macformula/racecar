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

namespace mcal {

using namespace stm32f767::periph;
ADCInput temp_sensor_adc_1{&hadc1, SENS_1_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_2{&hadc1, SENS_2_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_3{&hadc1, SENS_3_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_4{&hadc1, SENS_4_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_5{&hadc1, SENS_5_UC_IN_CHANNEL};
ADCInput temp_sensor_adc_6{&hadc1, SENS_6_UC_IN_CHANNEL};

PWMOutput fan_controller_pwm{&htim4, TIM_CHANNEL_1};
DigitalOutput debug_do_blue{NUCLEO_BLUE_LED_GPIO_Port, NUCLEO_BLUE_LED_Pin};
DigitalOutput debug_do_red{NUCLEO_RED_LED_GPIO_Port, NUCLEO_RED_LED_Pin};

}  // namespace mcal

namespace bindings {
const shared::periph::ADCInput& temp_sensor_adc_1 = mcal::temp_sensor_adc_1;
const shared::periph::ADCInput& temp_sensor_adc_2 = mcal::temp_sensor_adc_2;
const shared::periph::ADCInput& temp_sensor_adc_3 = mcal::temp_sensor_adc_3;
const shared::periph::ADCInput& temp_sensor_adc_4 = mcal::temp_sensor_adc_4;
const shared::periph::ADCInput& temp_sensor_adc_5 = mcal::temp_sensor_adc_5;
const shared::periph::ADCInput& temp_sensor_adc_6 = mcal::temp_sensor_adc_6;

const shared::periph::PWMOutput& fan_controller_pwm = mcal::fan_controller_pwm;
const shared::periph::DigitalOutput& debug_do_blue = mcal::debug_do_blue;
const shared::periph::DigitalOutput& debug_do_red = mcal::debug_do_red;

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

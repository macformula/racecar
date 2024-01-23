/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <string>

#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined in
 * cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {
mcal::periph::ADCInput temp_sensor_adc_1{&hadc1, SENS_1_UC_IN_CHANNEL};
mcal::periph::ADCInput temp_sensor_adc_2{&hadc1, SENS_2_UC_IN_CHANNEL};
mcal::periph::ADCInput temp_sensor_adc_3{&hadc1, SENS_3_UC_IN_CHANNEL};
mcal::periph::ADCInput temp_sensor_adc_4{&hadc1, SENS_4_UC_IN_CHANNEL};
mcal::periph::ADCInput temp_sensor_adc_5{&hadc1, SENS_5_UC_IN_CHANNEL};
mcal::periph::ADCInput temp_sensor_adc_6{&hadc1, SENS_6_UC_IN_CHANNEL};

mcal::periph::PWMOutput fan_controller_pwm{&htim4, TIM_CHANNEL_1};

mcal::periph::DigitalOutput debug_do_green{DEBUG_LED_GREEN_GPIO_Port,
                                           DEBUG_LED_GREEN_Pin};
mcal::periph::DigitalOutput debug_do_red{DEBUG_LED_RED_GPIO_Port,
                                         DEBUG_LED_RED_Pin};

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

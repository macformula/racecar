/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <string>

#include "gpio.h"
#include "main.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined in
 * cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {
mcal::periph::ADCInput temp_sensor_adc{&hadc1};
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

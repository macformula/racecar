#include "../../bindings.hpp"

#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f/periph/analog_input.hpp"
#include "mcal/stm32f/periph/gpio.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/gpio.hpp"
#include "stm32f4xx_hal.h"

namespace mcal {

stm32f::periph::AnalogInput analog_input{&hadc1, ADC_CHANNEL_10};
stm32f::periph::DigitalOutput green_led{green_led_GPIO_Port, green_led_Pin};
stm32f::periph::DigitalOutput red_led{red_led_GPIO_Port, red_led_Pin};
stm32f::periph::DigitalOutput blue_led{blue_led_GPIO_Port, blue_led_Pin};

}  // namespace mcal

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {

shared::periph::AnalogInput& analog_input = mcal::analog_input;
shared::periph::DigitalOutput& red_led = mcal::red_led;
shared::periph::DigitalOutput& blue_led = mcal::blue_led;
shared::periph::DigitalOutput& green_led = mcal::green_led;

void Init() {
    SystemClock_Config();
    HAL_Init();
    MX_ADC1_Init();
    MX_GPIO_Init();
}

}  // namespace bindings
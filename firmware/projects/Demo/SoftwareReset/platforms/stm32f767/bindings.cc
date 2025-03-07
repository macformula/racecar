#include "../../bindings.hpp"

#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/gpio.hpp"
#include "shared/periph/gpio.hpp"
#include "stm32f7xx_hal.h"

namespace mcal {

stm32f767::periph::DigitalOutput green_led{green_led_GPIO_Port, green_led_Pin};
stm32f767::periph::DigitalOutput red_led{red_led_GPIO_Port, red_led_Pin};
stm32f767::periph::DigitalOutput blue_led{blue_led_GPIO_Port, blue_led_Pin};
stm32f767::periph::DigitalInput reset_button{ResetButton_GPIO_Port,
                                             ResetButton_Pin};

}  // namespace mcal

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {

shared::periph::DigitalOutput& red_led = mcal::red_led;
shared::periph::DigitalOutput& blue_led = mcal::blue_led;
shared::periph::DigitalOutput& green_led = mcal::green_led;
shared::periph::DigitalInput& reset_button = mcal::reset_button;

void Init() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

void DelayMS(uint32_t ms) {
    HAL_Delay(ms);
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

}  // namespace bindings
// cubemx includes
#include "gpio.h"
#include "main.h"
#include "stm32f4xx_hal.h"

// project includes
#include "../../bindings.hpp"
#include "mcal/stm32f/periph/gpio.hpp"
#include "shared/periph/gpio.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}
namespace mcal {
using namespace stm32f::periph;

DigitalOutput indicator{GPIOB, GPIO_PIN_7};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalOutput& indicator = mcal::indicator;

void DelayMS(unsigned int ms) {
    HAL_Delay(ms);
}

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings
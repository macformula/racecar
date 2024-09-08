// cubemx includes
#include "gpio.h"
#include "main.h"
#include "stm32f7xx_hal.h"

// project includes
#include "bindings.h"
#include "mcal/stm32f767/periph/gpio.h"
#include "shared/periph/gpio.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}
namespace mcal {
using namespace stm32f767::periph;

DigitalOutput indicator{LedPin_GPIO_Port, LedPin_Pin};

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
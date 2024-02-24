/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
#include "gpio.h"
#include "main.h"

// fw includes
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
periph::DigitalInput button_di{ButtonPin_GPIO_Port, ButtonPin_Pin};
periph::DigitalOutput indicator_do{LedPin_GPIO_Port, LedPin_Pin};
periph::DigitalOutput indicator2_do{LedPin2_GPIO_Port, LedPin2_Pin};
}  // namespace mcal

namespace bindings {
const shared::periph::DigitalInput& button_di = mcal::button_di;
const shared::periph::DigitalOutput& indicator_do = mcal::indicator_do;
const shared::periph::DigitalOutput& indicator2_do = mcal::indicator2_do;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings

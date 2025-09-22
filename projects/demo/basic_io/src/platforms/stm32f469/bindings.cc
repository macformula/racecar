/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
#include "gpio.h"
#include "main.h"

// fw includes
#include "bindings.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "periph/gpio.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f;

DigitalInput button_di{ButtonPin_GPIO_Port, ButtonPin_Pin};
DigitalOutput indicator_do{LedPin_GPIO_Port, LedPin_Pin};
}  // namespace mcal

namespace bindings {
macfe::periph::DigitalInput& button_di = mcal::button_di;
macfe::periph::DigitalOutput& indicator_do = mcal::indicator_do;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings

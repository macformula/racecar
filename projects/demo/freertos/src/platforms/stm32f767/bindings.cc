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

DigitalInput button{BUTTON_GPIO_Port, BUTTON_Pin};
DigitalOutput blue_led{BLUE_LED_GPIO_Port, BLUE_LED_Pin};
DigitalOutput red_led{RED_LED_GPIO_Port, RED_LED_Pin};
}  // namespace mcal

namespace bindings {
macfe::periph::DigitalInput& button = mcal::button;
macfe::periph::DigitalOutput& blue_led = mcal::blue_led;
macfe::periph::DigitalOutput& red_led = mcal::red_led;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings

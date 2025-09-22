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

// This requires extern since it is not declared in a header, only defined
// in cubemx/../main.c
void SystemClock_Config();

// To handle the interrupt, override the __weak def from tm32f7xx_hal_gpio.c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN) {
    if (GPIO_PIN == Button_Pin) {
        AppInterruptHandler(0);
    }
}
}

namespace mcal {
using namespace stm32f;

DigitalOutput indicator{Led_GPIO_Port, Led_Pin};
}  // namespace mcal

namespace bindings {

macfe::periph::DigitalOutput& indicator = mcal::indicator;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings

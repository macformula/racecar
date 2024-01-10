/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include "gpio.h"
#include "main.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {
mcal::periph::DigitalInput button_di{ButtonPin_GPIO_Port, ButtonPin_Pin};
mcal::periph::DigitalOutput indicator_do{LedPin_GPIO_Port, LedPin_Pin};

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

}  // namespace bindings

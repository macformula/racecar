/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

namespace bindings {
mcal::periph::DigitalInput button_di{ButtonPin_GPIO_Port, ButtonPin_Pin};
mcal::periph::DigitalOutput indicator_do{LedPin_GPIO_Port, LedPin_Pin};
}  // namespace bindings

extern "C" {
void SystemClock_Config();
}

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    // MX_ADC1_Init();
    // MX_TIM1_Init();
}

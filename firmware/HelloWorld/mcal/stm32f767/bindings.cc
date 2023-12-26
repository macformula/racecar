/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include "gpio.h"
#include "main.h"

namespace bindings {
mcal::periph::DigitalInput button_di{USER_Btn_GPIO_Port, USER_Btn_Pin};
mcal::periph::DigitalOutput indicator_do{Led_GPIO_Port, Led_Pin};
}  // namespace bindings

extern "C" {
void SystemClock_Config();
}

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
}

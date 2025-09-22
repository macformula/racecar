/// @author Samuel Parent
/// @date 2024-04-16

// cubemx files
#include "can.h"
#include "gpio.h"
#include "main.h"

// fw includes
#include "bindings.hpp"
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "periph/can.hpp"
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

CanBase demo_can_base{&hcan1};
DigitalInput button{ButtonPin_GPIO_Port, ButtonPin_Pin};
}  // namespace mcal

namespace bindings {
macfe::periph::CanBase& demo_can_base = mcal::demo_can_base;
macfe::periph::DigitalInput& button = mcal::button;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_CAN1_Init();
    mcal::demo_can_base.Setup();
}

void TickBlocking(uint32_t ticks) {
    HAL_Delay(ticks);
}

}  // namespace bindings

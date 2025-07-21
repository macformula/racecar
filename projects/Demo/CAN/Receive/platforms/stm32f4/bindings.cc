/// @author Samuel Parent
/// @date 2024-04-16

// cubemx files
#include "can.h"
#include "gpio.h"
#include "main.h"

// fw includes
#include "../../bindings.hpp"
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

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
DigitalOutput indicator{LedPin_GPIO_Port, LedPin_Pin};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& demo_can_base = mcal::demo_can_base;
shared::periph::DigitalOutput& indicator = mcal::indicator;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_CAN1_Init();
    mcal::demo_can_base.Setup();
}

}  // namespace bindings

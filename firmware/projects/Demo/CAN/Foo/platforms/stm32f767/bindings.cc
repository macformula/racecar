/// @author Samuel Parent
/// @date 2024-04-16

// cubemx files
#include "can.h"
#include "gpio.h"
#include "main.h"

// fw includes
#include "mcal/stm32f767/periph/can.h"
#include "shared/periph/can.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f767::periph;

CanBase veh_can_base{&hcan3};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_CAN3_Init();
    mcal::veh_can_base.Setup();
}

void TickBlocking(uint32_t ticks) {
    HAL_Delay(ticks);
}

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan3) {
        mcal::veh_can_base.AddRxMessageToQueue();
    }
}
}

}  // namespace bindings

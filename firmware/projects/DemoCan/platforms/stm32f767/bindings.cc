// /// @author Samuel Parent
// /// @date 2024-01-16

// namespace mcal {
// using namespace stm32f767::periph;

// CanBase veh_can_base{&hcan1};
// }  // namespace mcal

// namespace bindings {
// shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

// void Initialize() {
//     mcal::veh_can_base.Setup();
// }
// }  // namespace bindings

/// @author Samuel Parent
/// @date 2024-04-16

// cubemx files
#include "can.h"
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

CanBase veh_can_base{&hcan2};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_CAN2_Init();
    mcal::veh_can_base.Setup();
}

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan2) {
        mcal::veh_can_base.AddRxMessageToQueue();
    }
}
}

}  // namespace bindings

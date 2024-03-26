/// @author Clara Wong
/// @date 2024-03-20

#include "bindings.h"
#include <iostream>


extern "C" {

void SystemClock_Config();

}
namespace bindings {

mcal::periph::CanBase veh_can_base{"vcan0"};

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_CAN_Init();
    veh_can_base.Setup();
}

} // namespace bindings

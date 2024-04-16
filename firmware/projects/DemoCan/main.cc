/// @author Samuel Parent
/// @date 2024-01-16

#include <sys/_stdint.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "generated/can/can_messages.h"
#include "generated/can/msg_registry.h"
#include "projects/DemoProject/platforms/stm32f767/cubemx/Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal.h"
#include "shared/comms/can/can_bus.h"
#include "stm32f7xx_hal.h"

// namespace os {
// extern void TickBlocking(uint32_t ticks);
// }

namespace bindings {
extern shared::periph::CanBase& veh_can_base;
extern void Initialize();
}  // namespace bindings

generated::can::VehMsgRegistry veh_can_registry{};

shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
};

int main(void) {
    bindings::Initialize();
    uint32_t tick_duration = 100;

    generated::can::TempSensors temp_sens_msg;
    generated::can::VehicleInfo veh_info_msg;

    float i = 0;
    while (true) {
        // os::TickBlocking(tick_duration);
        HAL_Delay(tick_duration);

        veh_can_bus.Update();

        veh_can_bus.Read(veh_info_msg);

        temp_sens_msg.sensor1 = veh_info_msg.requested_speed;
        temp_sens_msg.sensor2 = veh_info_msg.wheel_speed;
        temp_sens_msg.sensor3 = i++;
        temp_sens_msg.sensor4 = i++;
        temp_sens_msg.sensor5 = i++;
        temp_sens_msg.sensor6 = i++;

        veh_can_bus.Send(temp_sens_msg);
    }

    return 0;
}
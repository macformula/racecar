/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <iostream>
#include <thread>

#include "generated/can/can_messages.h"
#include "generated/can/msg_registry.h"
#include "shared/comms/can/can_bus.h"

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
    std::chrono::milliseconds duration(100);

    generated::can::TempSensors temp_sens_msg;
    generated::can::VehicleInfo veh_info_msg;

    int i = 0;
    while (1) {
        std::this_thread::sleep_for(duration);

        veh_can_bus.Update();

        veh_can_bus.Read(veh_info_msg);

        std::cout << "requested speed: " << veh_info_msg.requested_speed
                  << " actual speed: " << veh_info_msg.wheel_speed << std::endl;

        temp_sens_msg.sensor1 = i++;
        temp_sens_msg.sensor2 = i++;
        temp_sens_msg.sensor3 = i++;
        temp_sens_msg.sensor4 = i++;
        temp_sens_msg.sensor5 = i++;
        temp_sens_msg.sensor6 = i++;

        veh_can_bus.Send(temp_sens_msg);
    }

    return 0;
}
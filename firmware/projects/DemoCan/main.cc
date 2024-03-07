/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <iostream>
#include <thread>

#include "bindings.h"
#include "generated/can_messages.h"
#include "generated/msg_registry.h"
#include "shared/comms/can/can_bus.h"

namespace bindings {
extern mcal::periph::CanBase veh_can_base;
extern void Initialize();
}  // namespace bindings

generated::can::DemoCanVehMsgRegistry veh_can_regitry{};

shared::comms::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_regitry,
};

int main(void) {
    bindings::Initialize();
    std::chrono::milliseconds duration(1000);

    generated::can::TmsBroadcast tms_msg;
    generated::can::DebugLedOverride led_msg;

    int i = 0;
    while (1) {
        std::this_thread::sleep_for(duration);

        veh_can_bus.Update();

        veh_can_bus.Read(led_msg);

        std::cout << "led green: " << led_msg.set_green_led
                  << "led red: " << led_msg.set_red_led << std::endl;

        tms_msg.therm_module_num = i++;
        tms_msg.num_therm_enabled = i++;
        tms_msg.low_therm_value = i++;
        tms_msg.high_therm_value = i++;
        tms_msg.avg_therm_value = i++;
        tms_msg.high_therm_id = i++;
        tms_msg.low_therm_id = i++;
        tms_msg.checksum = i++;

        std::cout << "Sending tms message" << std::endl;

        veh_can_bus.Send(tms_msg);
    }

    return 0;
}
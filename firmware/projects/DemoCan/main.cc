/// @author Samuel Parent
/// @date 2024-01-16

#include "shared/comms/can/bus_manager.h"
#include "generated/can_messages.h"

#include "etl/unordered_map.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace bindings {
extern void Initialize();
extern mcal::periph::CanBase veh_can;
}  // namespace bindings

namespace generated::can {
extern etl::unordered_map rx_message_registry;
} // namespace generated::can

shared::comms::can::BusManager<kNumCanRxMesssages> bus_manager{
    bindings::veh_can,
    generated::can::rx_message::registry,
};

int main(void) {
    bindings::Initialize();
    std::chrono::milliseconds duration(1000);

    TmsBroadcast tms = {0};

    int i = 0;
    while (1) {
        std::this_thread::sleep_for(duration);

        tms.therm_module_num = i++;
        tms.num_therm_enabled = i++;
        tms.low_therm_value = i++;
        tms.high_therm_value = i++;
        tms.avg_therm_value = i++;
        tms.high_therm_id = i++;
        tms.low_therm_id = i++;
        tms.checksum = i++;
        
        std::cout << "Sending tms message" << std::endl;

        bus_manager.Send(tms);
    }

    return 0;
}
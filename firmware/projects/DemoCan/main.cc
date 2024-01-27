/// @author Samuel Parent
/// @date 2024-01-16

#include "bindings.h"
#include "generated/bus_manager.h"

#include <thread>
#include <chrono>

namespace bindings {
extern void Initialize();
extern mcal::periph::CanBase veh_can;
}  // namespace bindings

BusManager bus_manager{bindings::veh_can};

int main(void) {
    bindings::Initialize();
    std::chrono::milliseconds duration(10);

    struct shared::comms::can::can_header msg_hdr = {
        .can_id = 0x123,
        .data_len = 8,
        .is_extended_frame = false,
    };

    struct shared::comms::can::raw_can_msg msg = {
        .can_hdr = msg_hdr,
        .bytes = {0, 0, 0, 0, 0, 0, 0, 0},
    };

    int i = 0;
    while (1) {
        std::this_thread::sleep_for(duration);

        msg.bytes[i++%8]++;
        

        bindings::veh_can.Send(msg);
    }

    return 0;
}
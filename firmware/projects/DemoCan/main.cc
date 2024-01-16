/// @author Samuel Parent
/// @date 2024-01-16

#include "bindings.h"
#include "shared/comms/can/raw_can_msg.h"

#include <thread>
#include <chrono>

namespace bindings {
extern mcal::periph::CanBase veh_can;
extern void Initialize();
}  // namespace bindings



int main(void) {
    bindings::Initialize();
    std::chrono::seconds duration(1);

    struct can_header msg_hdr = {
        .can_id = 0x123,
        .is_extended_frame = false,
        .bytes = [0,0,0,0,0,0,0,0];
    };

    struct raw_can_msg msg = {
        can_header can_hdr;
        uint8_t data_len;
        uint8_t bytes[CAN_MSG_BYTES];
    };

    int i = 0;
    while (1) {
        std::this_thread::sleep_for(duration);

        msg.bytes[i++%8]++;
        

        veh_can.Send()
    }

    return 0;
}
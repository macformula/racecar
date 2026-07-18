/// @author Manush Patel
/// @date 2026-07

#include <string.h>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "stdio.h"

namespace bootloader {

static uint8_t firmware_buffer[RAM_BUFFER_SIZE_BYTES];  // size of ram func,
                                                        // already initialized
enum class State {
    IDLE,
    RECEIVING,
    VALIDATING,
    FLASHING
};

static State state = State::IDLE;

// __attribute__((weak)) void HoldSafeGpioStates() {
// };  // empty implementation unless overriden

// main command (run) will wait for 3 seconds, and if a message comes through
// will call another function that will intialize the periphs for canflash
int Run(void) {
    // need a function for holding safe gpio states [for can tx/rx, and status
    // led]

    switch (state) {
        case IDLE: {
            // HoldSafeGpioStates();
            uint32_t start_tick = HAL_GetTick();
            while ((HAL_GetTick() - start_tick) < 3000) {
                if (!RxInitiateCanFlash) {
                    break;
                }
                // send handshake to raspi
        veh_can_bus.Send(TxAcknowledgeCanFlashFC{.ecu = 0}));
            }

            state = State::Receiving
        } break;
    }

    case RECEIVING: {
        auto crc_msg_opt = veh_can.GetRxCanFlashCRC();

        if (crc_msg_opt.has_value()) {
            uint8_t crc = crc_msg_opt->CRC();
        }

        else {
            // Handle the case where the message is not received
        }
    }
        // receiving firmware in chunks stored in SRAM temporarily
};

static void TearDownHardware() {

};

}  // namespace bootloader

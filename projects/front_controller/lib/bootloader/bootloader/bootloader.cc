/// @author Manush Patel
/// @date 2026-07

#pragma once
#include <string.h>

#include "bindings.hpp"
#include "config.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "stdbool.h"
#include "stdio.h"
#include "stm32f7xx_hal_conf.h"

/// @brief  Bootloader Application Code
/// @note Implement a watchdog to avoid frozen states
namespace bootloader {

static uint8_t firmware_buffer[RAM_BUFFER_SIZE_BYTES];  // size of ram func,
                                                        // already initialized
enum class State {
    IDLE,
    RECEIVING,
    VALIDATING,
    FLASHING,
    FAULT,
};

static State state = State::IDLE;
static auto crc_msg_opt = 0;
static auto bytes_received = 0;

// __attribute__((weak)) void HoldSafeGpioStates() {
// };  // empty implementation unless overriden

// main command (run) will wait for 3 seconds, and if a message comes through
// will call another function that will intialize the periphs for canflash
int Run(void) {
    // need a function for holding safe gpio states [for can tx/rx, and status
    // led]

    while (1) {
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
            if (crc_msg_opt == 0) {
                auto crc_msg_opt = veh_can.GetRxCanFlashCRC();

                if (crc_msg_opt.has_value()) {
                    uint8_t crc32_rpi = crc_msg_opt->CRC();

                    uint8_t firmware_size = crc_msg_opt->Size();

                    if (firmware_size > RAM_BUFFER_SIZE_BYTES) {
                        state = State::FAULT;
                        break;
                    }
                }

                __disable_irq();  /// @note not sure if im able to do this

                // 256 kB -> 256,000 bytes / 8bytes per messages = 32,000
                // messages
                break;
            }

            auto firmware_chunks = veh_can.PopRxFirmwareChunks();

            if (firmware_chunks.has_value()) {
                if (bytes_received + 8 <= firwmware_size) {
                    firmware_buffer[bytes_received++] = chunks.index0();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index1();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index2();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index3();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index4();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index5();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index6();
                    firmware_buffer[bytes_received++] =
                        firmware_chunks->index7();
                }

                if (bytes_receieved == firmware_size) {
                    state = state::VALIDATING;
                }
            }

            else {
                // Handle the case where the message is not received
                break;
            }
        }

        case VALIDATING: {
            // validate firmware via CRC
            uint32_t crc32_host =
                crc32_iso_hdlc(firmware_buffer, firmware_size);

            if (crc32_host != crc32_rpi) {
                state = State::FAULT;
                veh_can.Send(TxCanFlashStatusFC{.state = State::FAULT});
                break;
            }

            veh_can.Send(TxCanFlashStatusFC{.state = FLASHING});
            state = State::FLASHING;
            break;
        }

        case FLASHING: {
            // use ram func section
            WriteFirmwaretoFlash();
        }

            // receiving firmware in chunks stored in SRAM temporarily
    }
};

static void TearDownHardware() {

};

uint32_t crc32_iso_hdlc(const void* data, size_t length) {
    const unsigned char* buf = (const unsigned char*)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc = (crc >> 1);
            }
        }
    }
    return crc ^ 0xFFFFFFFF;
}

__attribute__((section(".RamFunc")), noinline) bool WriteFirmwaretoFlash(
    const uint8_t* src, size_t len) {
    if (len > APP_FLASH_SIZE) return false;

    // disable interrupts when writing from SRAM to FLASH to avoid broken writes
    __disable_irq();

    // Unlock Flash
    if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
        // First Key Unlock
        FLASH->KEYR = FLASH_KEY1;

        // Second Key Unlock
        FLASH->KEYR = FLASH_KEY2;
    }

    /// @note: Need a way to verify if flash is unlocked

    if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
        // Flash not unlocked
        __enable_irq();
        return false;
    }

    // Flash is now unlocked, erase any current error flags to avoid default
    // fault This uses W1C (Write 1 to clear)
    FLASH->SR = (FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_WRPERR |

                 FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_ERSERR);

    // continue writing within this loop
    while (FLASH->SR & FLASH_SR_BUSY) {
        /// @note double check if these are all the errors possible on F7
        uint32_t error_mask =
            (FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_WRPERR |

             FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_ERSERR);

        if (FLASH->SR & error_mask) {
            // save error code that was triggered
            uint32_t active_errors = FLASH->SR & error_mask;

            // clear faults
            FLASH->SR = active_errors;
        }

        else {
            // ready to flash
        }
    }
}

}  // namespace bootloader

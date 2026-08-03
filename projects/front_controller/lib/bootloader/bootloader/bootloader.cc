/// @author Manush Patel
/// @date 2026-07

#include <string.h>

#include "atomic_buffer.hpp"
#include "bindings.hpp"
#include "can/bus.hpp"
#include "can/msg.hpp"
#include "config.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "periph/can.hpp"

#ifdef CRC
#undef CRC
#endif

/// @brief  Bootloader Application Code
/// @note Implement a watchdog to avoid frozen states
namespace bootloader {

using namespace generated::can;

static VehBus veh_can{bindings::veh_can_base};

static uint8_t firmware_buffer[RAM_BUFFER_SIZE_BYTES];  // size of ram func,
                                                        // already initialized
enum class State {
    IDLE,
    RECEIVING,
    VALIDATING,
    FLASHING,
    FAULT,
};

enum class FailurePoint {
    NONE = 0,

    // Firmware reception / validation
    SIZE_INVALID,
    CRC_INVALID,

    // Flash operation failures
    FLASH_UNLOCK,
    FLASH_ERASE,
    FLASH_PROGRAM,
    FLASH_VERIFY,
};

static FailurePoint failure_point = FailurePoint::NONE;
static State state = State::IDLE;
static bool metadata_received = false;
static uint32_t bytes_received = 0;
static uint8_t crc32_rpi;
static uint32_t firmware_size;

uint32_t crc32_iso_hdlc(const void* data, size_t length);
bool WriteFirmwaretoFlash(const uint8_t* src, size_t len);

// __attribute__((weak)) void HoldSafeGpioStates() {
// };  // empty implementation unless overriden

// main command (run) will wait for 3 seconds, and if a message comes through
// will call another function that will intialize the periphs for canflash
int Run(void) {
    // need a function for holding safe gpio states [for can tx/rx, and status
    // led

    while (1) {
        switch (state) {
            case bootloader::State::IDLE: {
                // HoldSafeGpioStates();
                uint32_t start_tick = HAL_GetTick();
                while ((HAL_GetTick() - start_tick) < 3000) {
                    if (veh_can.GetRxInitiateCanFlash()) {
                        veh_can.Send(TxAcknowledgeCanFlashFC{.ecu = 0});
                        state = State::RECEIVING;
                    }
                }

            } break;

            case bootloader::State::RECEIVING: {
                if (!metadata_received) {
                    auto crc_msg_opt = veh_can.GetRxCanFlashCRC();

                    if (crc_msg_opt.has_value()) {
                        crc32_rpi = crc_msg_opt->CRC();

                        firmware_size = crc_msg_opt->Size();

                        if (firmware_size == 0 ||
                            firmware_size > RAM_BUFFER_SIZE_BYTES) {
                            failure_point = FailurePoint::SIZE_INVALID;
                            state = State::FAULT;
                            break;
                        }

                        metadata_received = true;
                    }

                    // 256 kB -> 256,000 bytes / 8bytes per messages = 32,000
                    // messages
                    break;
                }

                auto firmware_chunks = veh_can.PopRxFirmwareChunks();

                if (firmware_chunks.has_value()) {
                    if (bytes_received + 8 <= firmware_size) {
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte0();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte1();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte2();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte3();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte4();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte5();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte6();
                        firmware_buffer[bytes_received++] =
                            firmware_chunks->byte7();
                    }

                    if (bytes_received == firmware_size) {
                        state = State::VALIDATING;
                    }
                }

                else {
                    // Handle the case where the message is not received
                    break;
                }
            }

            case bootloader::State::VALIDATING: {
                // validate firmware via CRC
                uint32_t crc32_host =
                    crc32_iso_hdlc(firmware_buffer, firmware_size);

                if (crc32_host != crc32_rpi) {
                    failure_point = FailurePoint::CRC_INVALID;
                    state = State::FAULT;
                    veh_can.Send(TxCanFlashStatusFC{
                        .state = TxCanFlashStatusFC::State_t::FAULT,
                    });
                    break;
                }

                veh_can.Send(TxCanFlashStatusFC{
                    .state = TxCanFlashStatusFC::State_t::FLASHING,
                });
                state = State::FLASHING;
                break;
            }

            case bootloader::State::FLASHING: {
                // use ram func section
                bool success =
                    WriteFirmwaretoFlash(firmware_buffer, bytes_received);

                if (success) {
                    FLASH->CR |= FLASH_CR_LOCK;

                    NVIC_SystemReset();  // reset cpu and continue with new
                                         // firmware
                } else {
                    state = State::FAULT;
                }
            } break;

            case bootloader::State::FAULT: {
                // determine recoverability, if flash is touched, manually
                // reflash prior can be recovered by nvic system reset()

                switch (failure_point) {
                        // clang-format off
                    case FailurePoint::CRC_INVALID:
                    case FailurePoint::FLASH_UNLOCK:
                    case FailurePoint::SIZE_INVALID: {
                        /// @note reminder: need to change for LVC, TMS
                        veh_can.Send(TxCANFlashFaultFC{
                            .fault_type = static_cast<
                                TxCANFlashFaultFC::FaultType_t>(failure_point),
                        });
                        NVIC_SystemReset();
                    } break;

                    case FailurePoint::FLASH_ERASE:
                    case FailurePoint::FLASH_PROGRAM:
                    case FailurePoint::FLASH_VERIFY: {
                        // send command to dash
                        static bool fault_sent = false;
                        if (!fault_sent){
                            /// @note reminder: need to change for LVC, TMS
                            veh_can.Send(TxCANFlashFaultFC{
                                .fault_type = static_cast<
                                    TxCANFlashFaultFC::FaultType_t>(
                                    failure_point),
                            });
                            fault_sent = true;
                        }
                    } break;

                    case FailurePoint::NONE:
                        break;

                        // clang-format on
                }
            } break;
        }
    }
}

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

bool EraseSector(uint32_t sector_number) {
    // Single sector erase function
    // Set PSIZE (parallelism field), clear+set SNB (which sector to erase)
    // Set STRT, __DSB(), poll BSY, check ERSERR, return true or false

    FLASH->CR &= ~FLASH_CR_PSIZE_Msk;  // clears bits 8 and 9
    FLASH->CR |= FLASH_CR_PSIZE_1;     // sets bit 9 only → field = 10 (word)

    FLASH->CR &= ~FLASH_CR_SNB_Msk;
    FLASH->CR |= (sector_number << FLASH_CR_SNB_Pos);

    // FLASH_CR_SER - mode switch to sector erase
    FLASH->CR |= FLASH_CR_SER;

    FLASH->SR = FLASH_SR_ERSERR;  // W1C — clear any stale error flag before
                                  // triggering

    // FLASH_CR_STRT - the trigger
    FLASH->CR |= FLASH_CR_STRT;

    // data synchronization buffer
    __DSB();

    while (FLASH->SR & FLASH_SR_BSY) {
        // feed watchdog here
    }

    if (FLASH->SR & FLASH_SR_ERSERR) {
        failure_point = FailurePoint::FLASH_ERASE;
        return false;  // erase failed
    } else {
        return true;
    }
}

bool VerifyFlash(const uint8_t* flash_addr, const uint8_t* ram_buffer,
                 size_t len) {
    return memcmp(flash_addr, ram_buffer, len) == 0;
}

bool WriteDoubleWord(uint32_t address, const uint8_t* data) {
    uint32_t* dest = (uint32_t*)address;

    const uint32_t* src_words = (const uint32_t*)data;

    // program flash
    FLASH->CR |= FLASH_CR_PG;

    FLASH->CR &= ~FLASH_CR_PSIZE_Msk;  // clears bits 8 and 9
    FLASH->CR |= FLASH_CR_PSIZE_1;     // sets bit 9 only → field = 10 (word)

    // clear error flags
    FLASH->SR = FLASH_SR_PGAERR | FLASH_SR_PGPERR;

    dest[0] = src_words[0];
    dest[1] = src_words[1];

    while (FLASH->SR & FLASH_SR_BSY) {
        // wait, feed watchdog
    }

    if ((FLASH->SR & FLASH_SR_PGPERR) || (FLASH->SR & FLASH_SR_PGAERR)) {
        failure_point = FailurePoint::FLASH_PROGRAM;
        return false;
    }

    return true;
}

__attribute__((section(".RamFunc"), noinline)) bool WriteFirmwaretoFlash(
    const uint8_t* src, size_t len) {
    if (len == 0 || len > FLASH_SIZE) {
        failure_point = FailurePoint::SIZE_INVALID;
        return false;
    }
    // disable interrupts when writing from SRAM to FLASH to avoid broken
    // writes
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
        failure_point = FailurePoint::FLASH_UNLOCK;
        __enable_irq();
        return false;
    }

    // Flash is now unlocked, erase any current error flags to avoid default
    // fault This uses W1C (Write 1 to clear)
    FLASH->SR = (FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_WRPERR |

                 FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_ERSERR);

    // continue writing within this loop
    while (FLASH->SR & FLASH_SR_BSY) {
        // feed watchdog here
    }

    /// @note double check if these are all the errors possible on F7
    uint32_t error_mask = (FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_WRPERR |

                           FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_ERSERR);

    if (FLASH->SR & error_mask) {
        // save error code that was triggered
        uint32_t active_errors = FLASH->SR & error_mask;

        // clear faults
        FLASH->SR = active_errors;
        failure_point = FailurePoint::FLASH_ERASE;
        __enable_irq();
        return false;
    }

    else {
        // ready to flash

        size_t covered = 0;
        for (size_t i = 0; i < KNUMSECTORS && covered < len; i++) {
            // erase flash sectors dependent on size of firmware
            if (!EraseSector(i)) {
                __enable_irq();
                return false;
            }
            covered += kSectors[i];
        }
    }

    // program flash
    for (size_t offset = 0; offset < len; offset += 8) {
        bool write_word = WriteDoubleWord(FLASH_START + offset, src + offset);

        if (!write_word) {
            __enable_irq();
            return false;
        }
    }

    // memcmp section to verify flash region == ram region
    if (VerifyFlash(reinterpret_cast<const uint8_t*>(FLASH_START),
                    firmware_buffer, len)) {
        __enable_irq();
        return true;
    } else {
        failure_point = FailurePoint::FLASH_VERIFY;
        __enable_irq();
        return false;
    }
}
}  // namespace bootloader

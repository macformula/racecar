

#include "lvbms.hpp"

// #include <array>
#include <cstring>
#include <iostream>

#include "periph/spi.hpp"

namespace macfe::lv {
macfe::lv::LvBms::LvBms(macfe::periph::SpiMaster& spi) : spi_(spi) {}

/**
 * @note The ADI codes are a little more intricate, and need to be looked into
 * @note ADI1, ADI2, ADV have dummy values, so this will need to change
 */

/**
 * @note everytime we utilize a read command, we should check for the CCNT
 * frames, and check them with our count. This will tell us if any commands may
 * have been ignored. Increment expected CCNT each time a write or command
 * function has been called
 **/

// Helper Functions -----------------------------------------------------
std::array<uint8_t, 2> LvBms::splitMessage(uint16_t cmd) {
    uint8_t hi = (cmd >> 8) & 0xFF;
    uint8_t lo = cmd & 0xFF;

    std::array<uint8_t, 2> split = {hi, lo};
    return split;
}

void LvBms::sendControlCmd(LvBms::commandCode cmdCode) {
    std::array<uint8_t, 2> cmd = LvBms::splitMessage(uint16_t(cmdCode));
    uint8_t tx[4] = {0};
    tx[0] = cmd[0];  // HI
    tx[1] = cmd[1];  // LO
    // Command PEC for transmit
    uint16_t pec = commandPec15(tx, 2);
    tx[2] = (pec >> 8) & 0xFF;
    tx[3] = pec & 0xFF;

    spi_.Transmit(tx, 4);
    expectedCCNT =
        (expectedCCNT % 63) + 1;  // Increment CCNT for Command Functions
}

bool LvBms::sendReadCmd(LvBms::commandCode cmdCode) {
    // Read Command: Command Bytes + Command PEC
    std::array<uint8_t, 2> cmd = LvBms::splitMessage(uint16_t(cmdCode));
    // Frame 1: Sends Read Command
    uint8_t tx[12] = {0};
    uint8_t rx[12] = {0};

    tx[0] = cmd[0];
    tx[1] = cmd[1];

    // Compute PEC15 for command
    uint16_t pec = commandPec15(tx, 2);
    tx[2] = (pec >> 8) & 0xFF;
    tx[3] = pec & 0xFF;

    for (int i = 4; i < 12; i++) {
        tx[i] = 0xFF;
    }

    spi_.TransmitReceive(tx, rx, 12);

    // Check returned PEC10
    uint16_t rxPec = (rx[10] << 8) | rx[11];

    // CCNT and PEC must be identical
    actualCCNT = (rx[10] >> 2) & 0x3F;

    uint16_t calcPec = dataPec10(&rx[4], 6);

    if (actualCCNT != expectedCCNT) {
        // resend the message 5 times, but each time check if CCNT has
        // increased, indicating a message sent
        uint8_t prevCCNT = actualCCNT;
        for (int i = 0; i < 5; i++) {
            spi_.TransmitReceive(tx, rx, 12);
            rxPec = (rx[10] << 8) | rx[11];
            actualCCNT = (rx[10] >> 2) & 0x3F;
            calcPec = dataPec10(&rx[4], 6);
            if (actualCCNT > prevCCNT) {  // this means the bms successfully
                                          // received atleast one message
                break;
            }
        }
    }
    if (rxPec != calcPec) {
        // resend the message once, this error should not happen often and very
        // rarely twice
        spi_.TransmitReceive(tx, rx, 12);
    }

    // return true if dpec matches
    return (rxPec == calcPec);
}

bool LvBms::sendWriteCmd(LvBms::commandCode cmdCode, const uint8_t data[6]) {
    // Write Command: 2 Command + 2 PEC15 + 6 Data + 2 PEC10 = 12 bytes
    std::array<uint8_t, 2> cmd = LvBms::splitMessage(uint16_t(cmdCode));
    // Command bytes
    uint8_t tx[12];
    tx[0] = cmd[0];
    tx[1] = cmd[1];

    // Command PEC15 bytes
    uint16_t pec = commandPec15(tx, 2);
    tx[2] = (pec >> 8) & 0xFF;
    tx[3] = pec & 0xFF;

    // Copy Data (6 bytes)
    std::memcpy(&tx[4], data, 6);

    // Data PEC10
    uint16_t dataPec = dataPec10(&tx[4], 6);
    tx[10] = (dataPec >> 8) & 0xFF;
    tx[11] = dataPec & 0xFF;

    // Sends all 12 bytes
    spi_.Transmit(tx, 12);

    expectedCCNT = (expectedCCNT % 63) + 1;  // Increment CCNT for Write
                                             // Commands

    return true;
}

uint16_t LvBms::commandPec15(const uint8_t* data, int len) const {
    // Packet Error Checker (CRC)
    uint16_t rem = 0x0010;
    uint16_t msbMask = 0x4000;  // bit 14
    constexpr uint16_t poly =
        0x4599;  // x^15 +x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1
    for (int i = 0; i < len; i++) {
        rem ^= static_cast<uint16_t>(data[i]) << 7;  // shifts to 15 bits, so
                                                     // XOR
        for (int b = 0; b < 8; b++) {
            bool top = rem & msbMask;  // checks for leftmost bit = 1
            rem <<= 1;
            if (top) rem ^= poly;
            rem &= 0x7FFF;  // forces remainder to be 15 bits
        }
    }

    return rem;
}

uint16_t LvBms::dataPec10(const uint8_t* data, int len) const {
    uint16_t rem = 0x20;                  // 00000010000 (10-bit initial value)
    constexpr uint16_t msbMask = 0x0200;  // bit9
    constexpr uint16_t poly = 0x8F;       // x^10 + x^7 + x^3 + x^2 + x^1 + 1
    for (int i = 0; i < len; i++) {
        rem ^= static_cast<uint16_t>(data[i]) << 2;
        for (int b = 0; b < 8; b++) {
            bool top = rem & msbMask;
            rem <<= 1;
            if (top) rem ^= poly;
            rem &= 0x3FF;  // keep CRC to 10 bits
        }
    }
    return rem;
}

}  // namespace macfe::lv
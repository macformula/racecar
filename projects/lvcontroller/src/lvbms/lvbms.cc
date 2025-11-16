

#include "lvbms.hpp"

#include <cstring>
#include <iostream>

#include "periph/spi.hpp"

namespace macfe::lv {
macfe::lv::LvBms::LvBms(macfe::periph::SpiMaster& spi) : spi_(spi) {}

/**
 * @note The ADI codes are a little more intricate, and need to be looked into
 * @note ADI1, ADI2, ADV have dummy values, so this will need to change
 */

enum class commandCode : uint16_t {
    SRST = 0x27,
    RSTCC = 0x2E,
    SNAP = 0x2D,
    UNSNAP = 0x2F,
    ADI1 = 0x00,
    ADI2 = 0x00,
    ADV = 0x00,
    ADX = 0x530,
    CLRI = 0x711,
    CLRA = 0x714,
    CLRVX = 0x712,
    CLRO = 0x713,
    CLRFLAG = 0x717,
    RDFLAG = 0x72,  // Includes ERR code, set to 1
    RDSTAT = 0x34,
    RDI = 0x12,
    RDVB = 0x13,
    RDIACC = 0x44,
    RDVBACC = 0x46,
    RDIVB1 = 0x14,
    RDIVB1ACC = 0x48,
    RDV1A = 0xA,
    RDV1B = 0x9,
    RDV1C = 0x3,
    RDV1D = 0xB,
    RDV2A = 0x7,
    RDV2B = 0xD,
    RDV2C = 0x5,
    RDV2D = 0x1F,
    RDV2E = 0x25,
    RDXA = 0x30,
    RDXB = 0x31,
    RDXC = 0x33,
    RDOC = 0xB,
    RDSID = 0x2C,
    RDCFGA = 0x2,
    RDCFGB = 0x26,
    RDCOMM = 0x722,
    WRCFGA = 0x1,
    WRCFGB = 0x24,
    WRCOMM = 0x721,
    STCOMM = 0x723,
    PLADC = 0x718,
    PLI1 = 0x71C,
    PLI2 = 0x71D,
    PLV = 0x71E,
    PLX = 0x71F,
    RDALLI = 0xC,
    RDALLA = 0x4C,
    RDALLV = 0x35,
    RDALLR = 0x11,
    RDALLX = 0x51,
    RDALLC = 0x10,
};

bool LvBms::resetCommandCounter() {
    uint16_t cmd = static_cast<uint16_t>(commandCode::RSTCC);
    uint8_t hi = (cmd >> 8) & 0xFF;  // RSTCC command (top half)
    uint8_t lo = cmd & 0xFF;         // low half
    uint8_t data[6];
    return sendReadCmd(hi, lo, data);
}

bool LvBms::sendReadCmd(uint8_t hi, uint8_t lo, uint8_t out[6]) {
    // Read Command: Command Bytes + Command PEC

    // Frame 1: Sends Read Command
    uint8_t tx[12] = {0};
    uint8_t rx[12] = {0};

    tx[0] = hi;
    tx[1] = lo;

    // Compute PEC15 for command
    uint16_t pec = commandPec15(tx, 2);
    tx[2] = (pec >> 8) & 0xFF;
    tx[3] = pec & 0xFF;

    for (int i = 4; i < 12; i++) {
        tx[i] = 0xFF;
    }

    spi_.TransmitReceive(tx, rx, 12);

    // Copy recieved data bytes
    std::memcpy(out, &rx[4], 6);

    // Check returned PEC10
    uint16_t rxPec = (rx[10] << 8) | rx[11];
    uint16_t calcPec = dataPec10(&rx[4], 6);

    // return true if dpec matches
    return (rxPec == calcPec);
}

bool LvBms::sendWriteCmd(uint8_t hi, uint8_t lo, const uint8_t data[6]) {
    // Write Command: 2 Command + 2 PEC15 + 6 Data + 2 PEC10 = 12 bytes

    // Command bytes
    uint8_t tx[12];
    tx[0] = hi;
    tx[1] = lo;

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
    constexpr uint16_t poly = 0x2D1;      // x^10 + x^7 + x^5 + x^4 + x^3 + 1
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
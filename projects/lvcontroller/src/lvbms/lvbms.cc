

#include "lvbms.hpp"

#include <iostream>

#include "periph/spi.hpp"

namespace macfe::lv {
macfe::lv::LvBms::LvBms(macfe::periph::SpiMaster& spi) : spi_(spi) {}

bool LvBms::resetCommandCounter() {
    uint8_t hi = (0x052E >> 8) & 0xFF;  // RSTCC command
    uint8_t lo = 0x052E & 0xFF;
    return sendCmd(hi, lo);
}

bool LvBms::sendCmd(uint8_t hi, uint8_t lo) {
    uint8_t tx[4] = {hi, lo, 0, 0};
    uint16_t pec = pec15(tx, 2);
    tx[2] = (pec >> 8) & 0xFF;
    tx[3] = pec & 0xFF;

    spi_.Transmit(tx, 4);
    return true;
}

uint16_t LvBms::pec15(const uint8_t* data, int len) const {
    // Packet Error Checker (CRC)
    uint16_t rem = 0x0010;
    constexpr uint16_t poly = 0x4599;
    for (int i = 0; i < len; i++) {
        rem ^= static_cast<uint16_t>(data[i]) << 7;
        for (int b = 0; b < 8; b++) {
            bool top = rem & 0x4000;  // checks for leftmost bit = 1
            rem <<= 1;
            if (top) rem ^= poly;
            rem &= 0x7FFF;
        }
    }

    return rem;
}
}  // namespace macfe::lv
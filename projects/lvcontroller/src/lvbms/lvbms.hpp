#pragma once
#include <cstdint>

namespace macfe::periph {
class SpiMaster;
}

namespace macfe::lv {

class LvBms {
public:
    LvBms(macfe::periph::SpiMaster&);

    bool init();
    bool reset();
    bool resetCommandCounter();  // RSTCC

private:
    macfe::periph::SpiMaster& spi_;
    bool sendReadCmd(uint8_t hi, uint8_t lo, uint8_t out[6]);
    bool sendWriteCmd(uint8_t hi, uint8_t lo, const uint8_t data[6]);

    uint16_t commandPec15(const uint8_t* data, int len) const;
    uint16_t dataPec10(const uint8_t* data, int len) const;
};

}  // namespace macfe::lv
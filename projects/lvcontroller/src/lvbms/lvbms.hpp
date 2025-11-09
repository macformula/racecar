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
    bool sendCmd(uint8_t hi, uint8_t lo);
    uint16_t pec15(const uint8_t* data, int len) const;
};

}  // namespace macfe::lv
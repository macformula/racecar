#pragma once

namespace macfe::periph {
class SpiMaster;
}

namespace macfe::lv {

class LvBms {
public:
    LvBms(macfe::periph::SpiMaster&);

private:
    macfe::periph::SpiMaster& spi_;
};

}  // namespace macfe::lv
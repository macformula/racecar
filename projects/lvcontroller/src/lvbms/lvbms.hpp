#pragma once
#include <array>
#include <cstdint>

namespace macfe::periph {
class SpiMaster;
}

namespace macfe::lv {

class LvBms {
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

public:
    LvBms(macfe::periph::SpiMaster&);

private:
    macfe::periph::SpiMaster& spi_;
    uint8_t expectedCCNT = 0;  // tracks expected command counter
    uint8_t actualCCNT = 0;

    bool sendReadCmd(commandCode cmdCode);
    bool sendWriteCmd(commandCode cmdCode, const uint8_t data[6]);
    void sendControlCmd(commandCode cmdCode);
    std::array<uint8_t, 2> splitMessage(uint16_t cmd);

    uint16_t commandPec15(const uint8_t* data, int len) const;
    uint16_t dataPec10(const uint8_t* data, int len) const;
};

}  // namespace macfe::lv
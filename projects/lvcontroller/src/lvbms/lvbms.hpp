#pragma once
#include <array>
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

    // Command Codes
    void softwareReset();
    void resetCommandCounter();      // RSTCC
    void freezeResultRegisters();    // SNAP
    void unfreezeResultRegisters();  // UNSNAP
    void clearIxVBxADCResults();     // CLRI
    void clearAccumalators();        // CLRA
    void clearVxADCResults();        // CLRVX
    void clearOCxADCResults();       // CLRO

    // Read Commands
    // -----------------------
    void RDFLAG();
    void RDSTAT();
    void RDI();
    void RDV8();
    void RDIACC();
    void RDVBACC();
    void RDIVB1();
    void RDIB1ACC();

    // ADCV1
    void RDV1A();
    void RDV1B();
    void RDV1C();
    void RDV1D();

    // ADCV2
    void RDV2A();
    void RDV2B();
    void RDV2C();
    void RDV2D();
    void RDV2E();

    // AUX ADC
    void RDXA();
    void RDXB();
    void RDXC();

    void RDSID();
    void RDCFGA();
    void RDCFGB();

    // Write Commands
    // -----------------------
    void CLRFLAG();

private:
    macfe::periph::SpiMaster& spi_;
    uint8_t expectedCCNT = 0;  // tracks expected command counter
    uint8_t actualCCNT = 0;

    bool sendReadCmd(uint8_t hi, uint8_t lo, uint8_t out[6],
                     bool measurementRead);
    bool sendWriteCmd(uint8_t hi, uint8_t lo, const uint8_t data[6]);
    void sendControlCmd(std::array<uint8_t, 2> cmd);
    std::array<uint8_t, 2> splitMessage(uint16_t cmd);

    uint16_t commandPec15(const uint8_t* data, int len) const;
    uint16_t dataPec10(const uint8_t* data, int len) const;
};

}  // namespace macfe::lv
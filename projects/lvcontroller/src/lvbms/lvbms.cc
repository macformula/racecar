

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

// Helper Functions -----------------------------------------------------
std::array<uint8_t, 2> LvBms::splitMessage(uint16_t cmd) {
    uint8_t hi = (cmd >> 8) & 0xFF;
    uint8_t lo = cmd & 0xFF;

    std::array<uint8_t, 2> split = {hi, lo};
    return split;
}

void LvBms::sendControlCmd(std::array<uint8_t, 2> cmd) {
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

bool LvBms::sendReadCmd(uint8_t hi, uint8_t lo, uint8_t out[6]) {
    // Read Command: Comand Bytes + Command PEC

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

    // Check returned PEC10
    uint16_t rxPec = (rx[10] << 8) | rx[11];

    // CCNT and PEC must be identical
    actualCCNT = (rx[10] >> 2) & 0x3F;

    uint16_t calcPec = dataPec10(&rx[4], 6);

    if (actualCCNT != expectedCCNT) {
        // maybe define a special message back to lv controller
        // From now on, these values won't be the same... this could flood our
        // can bus
    }
    if (rxPec != calcPec) {
        // send back a specific message that the message was not sent through,
        // this would be a rare instance since PEC is pretty good
    }

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

// ------------------------------------------------------------------------------------

//  Command Codes
void LvBms::softwareReset() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::SRST));
    // Look for expected CCNT Reset
    sendControlCmd(cmd);
}

void LvBms::resetCommandCounter() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RSTCC));
    expectedCCNT = 0;  // reset
    sendControlCmd(cmd);
}

void LvBms::freezeResultRegisters() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::SNAP));

    sendControlCmd(cmd);
}

void LvBms::unfreezeResultRegisters() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::UNSNAP));
    sendControlCmd(cmd);
}

void LvBms::clearIxVBxADCResults() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::CLRI));

    sendControlCmd(cmd);
}

void LvBms::clearAccumalators() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::CLRA));

    sendControlCmd(cmd);
}

void LvBms::clearVxADCResults() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::CLRVX));

    sendControlCmd(cmd);
}

void LvBms::clearOCxADCResults() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::CLRO));

    sendControlCmd(cmd);
}
// --------------------------------------------------------

// Write Functions
void LvBms::CLRFLAG() {
    /*
    Used to reset individual bits in the flag register. This write function,
   like all, require 6 bytes sent back. Any bit of the data that is set to 1
   clears the related flag
    */
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::CLRFLAG));
    uint8_t output[6];
    sendWriteCmd(cmd[0], cmd[1], output);
}

// Read Functions

// non measurement read will pass in false -> will not increment CCNT

void LvBms::RDFLAG() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDFLAG));
    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);

    // Fault and Status Information is read from this command
}

void LvBms::RDSTAT() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDSTAT));
    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDI() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDI));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDVB() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDVB));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDIACC() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDIACC));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDVBACC() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDIACC));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDIVB1() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDIVB1));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDIVB1ACC() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDIVB1ACC));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV1A() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV1A));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV1B() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV1B));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV1C() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV1C));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV1D() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV1D));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV2A() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV2A));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV2B() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV2B));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV2C() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV2C));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV2D() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV2D));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDV2E() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDV2E));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDXA() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDXA));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDXB() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDXB));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDXC() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDXC));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDOC() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDOC));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDSID() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDSID));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDCFGA() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDCFGA));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDCFGB() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDCFGB));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::RDCOMM() {
    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDCOMM));

    uint8_t output[6];
    sendReadCmd(cmd[0], cmd[1], output);
}

void LvBms::readAllConfigFlags(uint8_t input[6]) {

    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDALLC));

    sendReadCmd(cmd[0], cmd[1], input);


}

void LvBms::readAllAUXADC(uint8_t input[6]) {

    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDALLX));

    sendReadCmd(cmd[0], cmd[1], input);

}

void LvBms::readAllExternalinputV2ADCResults(uint8_t input[6]) {

    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDALLR));

    sendReadCmd(cmd[0], cmd[1], input);

}


void LvBms::readAllExternalinputV1ADCResults(uint8_t input[6]) {

    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDALLV));

    sendReadCmd(cmd[0], cmd[1], input);

}


void LvBms::readIxADCandVbADCaccumulators(uint8_t input[6]) {
    freezeResultRegisters();

    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDALLA));

    sendReadCmd(cmd[0], cmd[1], input);

    unfreezeResultRegisters();

}

void LvBms::readIxADCandVbADCresults(uint8_t input[6]) {
    freezeResultRegisters();

    std::array<uint8_t, 2> cmd =
        LvBms::splitMessage(uint16_t(commandCode::RDALLI));

    sendReadCmd(cmd[0], cmd[1], input);

    unfreezeResultRegisters();

}



}  // namespace macfe::lv
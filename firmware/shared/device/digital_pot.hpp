/// @author Samuel Parent
/// @date 2025-02-04

#pragma once

#include <concepts>
#include <cstdint>

#include "shared/comms/i2c/msg.hpp"
#include "shared/periph/i2c.hpp"
#include "shared/util/device.hpp"

namespace shared::device {

/*
 * Circuit Topology:
 *   RA - Resistance between Terminal(A) and Terminal(W)
 *   RB - Resistance between Terminal(B) and Terminal(W)
 *
 *   Terminal(A)___/ ___
 *                      |
 *                      |
 *                     [RA]
 *                      |
 *                      |
 *                      |___/ ___Terminal(W)
 *                      |
 *                      |
 *                     [RB]
 *                      |
 *   Terminal(B)___/ ___|
 */
template <typename ResolutionT>
    requires std::unsigned_integral<ResolutionT>
class DigitalPotentiometer : public util::Device {
public:
    enum class Terminal {
        A,
        B,
        W,
    };

    virtual void SetPosition(uint8_t circuit_index, ResolutionT position) = 0;
    virtual ResolutionT GetPosition(uint8_t circuit_index) = 0;
    virtual void SetTerminalConnection(uint8_t circuit_index, Terminal terminal,
                                       bool connected) = 0;
    virtual bool GetTerminalConnection(uint8_t circuit_index,
                                       Terminal terminal) = 0;
    virtual void SetTerminalConnections(uint8_t circuit_index,
                                        bool terminalConnectedA,
                                        bool terminalConnectedB,
                                        bool terminalConnectedW) = 0;
};
}  // namespace shared::device

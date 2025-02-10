/// @author Samuel Parent
/// @date 2025-02-04

#pragma once

#include <concepts>
#include <cstdint>

#include "shared/comms/i2c/msg.hpp"
#include "shared/periph/i2c.hpp"
#include "shared/util/device.hpp"

namespace shared::device {

enum class DigitalPotTerminal {
    A,
    B,
    W,
};

/*
 * Circuit Topology:
 *   RA - Resistance between Terminal(A) and Terminal(W)
 *   RB - Resistance between Terminal(B) and Terminal(W)
 *
 *   Terminal(A)___/ ___
 *                      |
 *                      |_
 *                     |  |
 *                     |RA|
 *                     |__|
 *                      |
 *                      |____/ ___Terminal(W)
 *                      |_
 *                     |  |
 *                     |RB|
 *                     |__|
 *                      |
 *   Terminal(B)___/ ___|
 */
template <typename ResolutionT>
    requires(std::unsigned_integral<ResolutionT>)
class DigitalPotentiometerController : public util::Device {
public:
    virtual void SetPosition(uint8_t pot_index, ResolutionT position) = 0;
    virtual ResolutionT GetPosition(uint8_t pot_index) = 0;
    virtual bool GetTerminalConnection(uint8_t pot_index,
                                       DigitalPotTerminal terminal) = 0;
    virtual void GetTerminalConnections(uint8_t pot_index,
                                        bool* terminal_connected_a,
                                        bool* terminal_connected_b,
                                        bool* terminal_connected_w) = 0;
    virtual void SetTerminalConnections(uint8_t pot_index,
                                        bool terminal_connect_a,
                                        bool terminal_connect_b,
                                        bool terminal_connect_w) = 0;
    virtual void SetTerminalConnection(uint8_t pot_index,
                                       DigitalPotTerminal terminal,
                                       bool terminal_connect) = 0;
};

// Template deduction guide
template <typename ResolutionT>
DigitalPotentiometerController(DigitalPotentiometerController<ResolutionT>&)
    -> DigitalPotentiometerController<ResolutionT>;

template <typename ResolutionT>
    requires(std::unsigned_integral<ResolutionT>)
class DigitalPotentiometer {
public:
    DigitalPotentiometer(
        DigitalPotentiometerController<ResolutionT>& controller,
        uint8_t pot_index)
        : controller_(controller), pot_index_(pot_index) {}

    void SetPosition(ResolutionT position) {
        controller_.SetPosition(pot_index_, position);
    }

    ResolutionT GetPosition() {
        return controller_.GetPosition(pot_index_);
    }

    bool GetTerminalConnection(DigitalPotTerminal terminal) {
        return controller_.GetTerminalConnection(pot_index_, terminal);
    }

    void GetTerminalConnections(bool* terminal_connected_a,
                                bool* terminal_connected_b,
                                bool* terminal_connected_w) {
        controller_.GetTerminalConnections(pot_index_, terminal_connected_a,
                                           terminal_connected_b,
                                           terminal_connected_w);
    }

    void SetTerminalConnections(bool terminal_connect_a,
                                bool terminal_connect_b,
                                bool terminal_connect_w) {
        controller_.SetTerminalConnections(pot_index_, terminal_connect_a,
                                           terminal_connect_b,
                                           terminal_connect_w);
    }

    void SetTerminalConnection(DigitalPotTerminal terminal,
                               bool terminal_connect) {
        controller_.SetTerminalConnection(pot_index_, terminal, terminal_connect);
    }

private:
    DigitalPotentiometerController<ResolutionT>& controller_;
    const uint8_t pot_index_;
};

}  // namespace shared::device

/// @author Blake Freer
/// @date 2023-10-17

#ifndef MCAL_DIGITAL_INPUT_PORT_HPP
#define MCAL_DIGITAL_INPUT_PORT_HPP

#include "mcal_input/digital_input_base.hpp"
#include "mcal_port/port_pin.hpp"

namespace mcal {
namespace input {

/**
 * @brief Digital input from a physical pin
 */
class DigitalInputPin : public DigitalInput_Base {

private:
    port::port_pin pin;

public:
    DigitalInputPin(port::port_pin _port_pin) : pin(_port_pin) {}

    bool read() noexcept {
        return pin.read_bit();
    }
};

} // namespace input
} // namespace mcal

#endif
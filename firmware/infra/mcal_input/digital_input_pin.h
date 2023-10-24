/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_MCAL_DIGITAL_INPUT_PIN_H_
#define INFRA_MCAL_DIGITAL_INPUT_PIN_H_

#include "mcal_input/digital_input_base.h"
#include "mcal_port/port_pin.h"

namespace mcal {
namespace input {

/// @brief Digital input from a physical pin
class DigitalInputPin : public DigitalInputBase {
private:
    port::PortPin pin_;

public:
    DigitalInputPin(port::PortPin pin) : pin_(pin) {}

    bool read() noexcept { return pin_.ReadBit(); }
};

}  // namespace input
}  // namespace mcal

#endif
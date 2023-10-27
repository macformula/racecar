/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_MCAL_DIGITAL_INPUT_PIN_H_
#define INFRA_MCAL_DIGITAL_INPUT_PIN_H_

#include "mcal_input/digital_input_base.h"
#include "gpio.h"

namespace mcal {
namespace input {

/// @brief Digital input from a physical pin

template <typename pin>
class DigitalInputPin : public DigitalInputBase {

public:
	void init() {
		pin::init();
	}
    bool read() noexcept {
		return pin::read();
	}
};

}  // namespace input
}  // namespace mcal

#endif
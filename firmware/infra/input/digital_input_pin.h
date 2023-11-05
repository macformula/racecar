/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_INPUT_DIGITAL_INPUT_PIN_H_
#define INFRA_INPUT_DIGITAL_INPUT_PIN_H_

#include "input/digital_input_base.h"
#include "mcal_type.h"
#include "gpio.h"

namespace infra {
namespace input {


/// @brief Digital input from a physical pin
/// @tparam pin Must be a class conforming to the `GpioPin` concept
template <mcal::gpio::GpioPin pin>
class DigitalInputPin : public DigitalInputBase {

public:
	void Init() {
		pin::init();
	}
    bool Read() noexcept override  {
		return pin::Read();
	}
};

}  // namespace input
}  // namespace mcal

#endif
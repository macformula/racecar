/// @author Blake Freer
/// @date 2023-11-01

#ifndef INFRA_OUTPUT_DIGITAL_OUTPUT_PIN_H_
#define INFRA_OUTPUT_DIGITAL_OUTPUT_PIN_H_

#include "mcal_type.h"
#include "output/digital_output_base.h"

namespace infra {
namespace output {

template<mcal::gpio::GpioPin pin>
class DigitalOutputPin : public DigitalOutputBase {

public:
	void Init() {
		pin::Init();
	}
	void SetHigh() noexcept override {
		pin::Set();
	}
	void SetLow() noexcept override {
		pin::Clear();
	}
};

} // namespace output
} // namespace infra


#endif
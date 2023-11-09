/// @author Blake Freer
/// @date 2023-11-08

#ifndef INFRA_GPIO_DIGITAL_OUTPUT_H_
#define INFRA_GPIO_DIGITAL_OUTPUT_H_

#include "infra/util/peripheral.h"

namespace infra {
namespace gpio {

class DigitalOutput : public util::Peripheral {
public:
	virtual void SetHigh() noexcept = 0;
	
	virtual void SetLow() noexcept = 0;

	virtual void Set(bool value) noexcept = 0;
};

} // namespace gpio
} // namespace infra

#endif
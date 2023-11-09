/// @author Blake Freer
/// @date 2023-11-08

#ifndef INFRA_GPIO_DIGITA_INPUT_H_
#define INFRA_GPIO_DIGITA_INPUT_H_

#include "infra/util/peripheral.h"

namespace infra {
namespace gpio {

class DigitalInput : public util::Peripheral {
public:
	virtual bool Read() noexcept = 0;
};

} // namespace gpio
} // namespace infra

#endif
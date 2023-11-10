/// @author Blake Freer
/// @date 2023-11-09

#ifndef INFRA_ADC_ADC_INPUT_H_
#define INFRA_ADC_ADC_INPUT_H_


#include "stdint.h"
#include "infra/util/peripheral.h"

namespace infra {
namespace adc {

class ADCInput : public util::Peripheral {
public:
	virtual void Start() noexcept = 0;
	virtual uint32_t Read() = 0;
};

} // namespace adc
} // namespace infra

#endif
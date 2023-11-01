/// @author Blake Freer
/// @date 2023-11-01

#ifndef INFRA_OUTPUT_DIGITAL_OUTPUT_BASE_H_
#define INFRA_OUTPUT_DIGITAL_OUTPUT_BASE_H_

#include "output/output_base.h"

namespace infra {
namespace output {

class DigitalOutputBase : public OutputBase<bool>{
public:
	void Set(bool value) noexcept override final {
		if(value) {
			SetHigh();
		} else {
			SetLow();
		}
	}
	virtual void SetHigh() noexcept = 0;
	virtual void SetLow() noexcept = 0;
};

} // namespace output
} // namespace infra


#endif
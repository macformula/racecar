/// @author Blake Freer
/// @date 2023-11-09

#ifndef INFRA_PWM_PWM_OUTPUT_H_
#define INFRA_PWM_PWM_OUTPUT_H_

#include "infra/util/peripheral.h"

namespace infra {
namespace pwm {

class PWMOutput : public util::Peripheral {
public:
	virtual void Start() noexcept = 0;
	virtual void Stop() noexcept = 0;
	virtual void SetDutyCycle(float duty_cycle) = 0;
	virtual float GetDutyCycle() = 0;
};

} // namespace pwm
} // namespace infra

#endif
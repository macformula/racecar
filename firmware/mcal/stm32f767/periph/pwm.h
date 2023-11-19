/// @author Blake Freer
/// @date 2023-11-09

#ifndef MCAL_STM32F767_PERIPH_PWM_H_
#define MCAL_STM32F767_PERIPH_PWM_H_

#include <cstdint>
#include "stm32f7xx_hal_tim.h"

#include "shared/util/peripheral.h"

namespace mcal::periph {

class PWMOutput : public shared::util::Peripheral {

private:
	TIM_HandleTypeDef* htim_;
	uint32_t channel_;

public:
	PWMOutput(TIM_HandleTypeDef* htim, uint32_t channel) : htim_(htim), channel_(channel) {}


	void Start() {
		HAL_TIM_PWM_Start(htim_, channel_);
	}
	void Stop() {
		HAL_TIM_PWM_Stop(htim_, channel_);
	}

	void SetDutyCycle(float duty_cycle) {
		// clamp duty cycle between 0 and 100
		duty_cycle = (duty_cycle < 0.0f) ? 0.0f : (duty_cycle > 100.0f) ? 100.0f : duty_cycle;
		
		uint32_t pulse = uint32_t(duty_cycle / 100.0f * float(__HAL_TIM_GetAutoreload(htim_)));
		__HAL_TIM_SetCompare(htim_, channel_, pulse);
	}

	float GetDutyCycle() {
		uint32_t pulse = __HAL_TIM_GetCompare(htim_, channel_);
		uint32_t period = __HAL_TIM_GetAutoreload(htim_);

		return float(pulse) / float(period) * 100.0f;
	}

};

} // namespace mcal::periph

#endif
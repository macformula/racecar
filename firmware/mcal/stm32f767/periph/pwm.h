/// @author Blake Freer
/// @date 2023-11-09

#ifndef MCAL_STM32F767_PERIPH_PWM_H_
#define MCAL_STM32F767_PERIPH_PWM_H_

#include <cstdint>

#include "shared/periph/pwm.h"
#include "stm32f7xx_hal.h"

namespace mcal::periph {

class PWMOutput : public shared::periph::PWMOutput {
private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;

public:
    PWMOutput(TIM_HandleTypeDef* htim, uint32_t channel)
        : htim_(htim), channel_(channel) {}

    void Start() override {
        HAL_TIM_PWM_Start(htim_, channel_);
    }
    void Stop() override {
        HAL_TIM_PWM_Stop(htim_, channel_);
    }

    void SetDutyCycle(float duty_cycle) override {
        // clamp duty cycle between 0 and 100
        duty_cycle = (duty_cycle < 0.0f)     ? 0.0f
                     : (duty_cycle > 100.0f) ? 100.0f
                                             : duty_cycle;

        uint32_t pulse = uint32_t(duty_cycle / 100.0f *
                                  float(__HAL_TIM_GetAutoreload(htim_)));
        __HAL_TIM_SetCompare(htim_, channel_, pulse);
    }

    float GetDutyCycle() override {
        uint32_t pulse = __HAL_TIM_GetCompare(htim_, channel_);
        uint32_t period = __HAL_TIM_GetAutoreload(htim_);

        return float(pulse) / float(period) * 100.0f;
    }
};

}  // namespace mcal::periph

#endif
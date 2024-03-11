/// @author Blake Freer
/// @date 2023-11-09

#pragma once

#include <cstdint>

#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"
#include "stm32f7xx_hal.h"

namespace mcal::periph {

class PWMOutput : public shared::periph::PWMOutput {
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
        duty_cycle = shared::util::Clamper<float>::Evaluate(duty_cycle, 0, 100);

        uint32_t pulse = uint32_t(duty_cycle / 100.0f *
                                  float(__HAL_TIM_GetAutoreload(htim_)));
        __HAL_TIM_SetCompare(htim_, channel_, pulse);
    }

    float GetDutyCycle() override {
        uint32_t pulse = __HAL_TIM_GetCompare(htim_, channel_);
        uint32_t period = __HAL_TIM_GetAutoreload(htim_);

        return float(pulse) / float(period) * 100.0f;
    }

private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;
};

}  // namespace mcal::periph

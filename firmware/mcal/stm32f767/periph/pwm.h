/// @author Blake Freer
/// @date 2023-11-09

#pragma once

#include <algorithm>
#include <cstdint>

#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"
#include "stm32f7xx_hal.h"

namespace mcal::stm32f767::periph {

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
        // notice this sets the instance variable based on the argument
        duty_cycle_ =
            shared::util::Clamper<float>::Evaluate(duty_cycle, 0, 100);

        uint32_t pulse = uint32_t(duty_cycle_ / 100.0f *
                                  float(__HAL_TIM_GetAutoreload(htim_)));
        __HAL_TIM_SetCompare(htim_, channel_, pulse);
    }

    /**
     * @brief Get the duty cycle from the PWM registers.
     * @note This method returns the "most accurate" current duty cycle.
     *
     * @return float
     */
    float GetDutyCycle() override {
        uint32_t pulse = __HAL_TIM_GetCompare(htim_, channel_);
        uint32_t period = __HAL_TIM_GetAutoreload(htim_);

        return float(pulse) / float(period) * 100.0f;
    }

    void SetFrequency(float frequency) override {
        float frequency_ = std::max(kMinimumFrequency, frequency);
        uint32_t autoreload = static_cast<uint32_t>(
            static_cast<float>(GetTimerFrequency()) / frequency_);

        __HAL_TIM_SetAutoreload(htim_, autoreload);
    }

    float GetFrequency() override {
        float frequency = static_cast<float>(GetTimerFrequency()) /
                          static_cast<float>(__HAL_TIM_GetAutoreload(htim_));

        return frequency;
    }

private:
    TIM_HandleTypeDef* htim_;
    // stm32f767 has a 16-bit autoreload register -> min frequency = 1/65535
    static constexpr float kMinimumFrequency = 0.000015259022f;
    uint32_t channel_;
    float duty_cycle_ = 0;

    uint32_t GetTimerFrequency() {
        uint32_t tickFreq = HAL_GetTickFreq();

        return tickFreq;
    }
};

}  // namespace mcal::stm32f767::periph

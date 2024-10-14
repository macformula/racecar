/// @author Blake Freer
/// @date 2023-11-09

#pragma once

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
     * @brief Quickly get the duty cycle which was last sent to this device.
     * @note Compared to GetHardwareDutyCycle, this method is faster (doesn't
     * read registers) and is less likely to cause issues when performing
     * arithmetic since the hardware duty cycle is limited by integer
     * resolution.
     *
     * @return float
     */
    float GetDutyCycle() override {
        return duty_cycle_;
    }

    /**
     * @brief Get the duty cycle from the PWM registers.
     * @note Compared to GetDutyCycle(), this method returns the "most accurate"
     * current duty cycle.
     *
     * @return float
     */
    float GetHardwareDutyCycle() {
        uint32_t pulse = __HAL_TIM_GetCompare(htim_, channel_);
        uint32_t period = __HAL_TIM_GetAutoreload(htim_);

        return float(pulse) / float(period) * 100.0f;
    }

    void SetFrequency(float frequency) override {
        frequency_ = std::max((float) 0, frequency);
        uint32_t autoreload = 1/frequency_;

        __HAL_TIM_SetAutoreload(htim, autoreload);
    }

    float GetFrequency() override {
        return frequency_;
    }

private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;
    float duty_cycle_ = 0;
    float frequency_;
};

}  // namespace mcal::stm32f767::periph

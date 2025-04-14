/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#pragma once

#include <array>

#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/lookup_table.hpp"

class FanContoller {
public:
    FanContoller(shared::periph::PWMOutput& pwm) : pwm_(pwm) {}

    void Start(float initial_power) {
        pwm_.Start();
        pwm_.SetDutyCycle(100.f - initial_power);
    }

    void Update(float temperature, float update_period_ms) {
        using LUT = shared::util::LookupTable<float>;

        float desired_power = LUT::Evaluate(temp_to_power_, temperature);
        // convert pwm = 100 - power since the fan runs on inverse logic
        // ex. pwm=20% => fan is running at 80%
        float desired_pwm = 100.f - desired_power;

        float current_pwm = pwm_.GetDutyCycle();
        float delta_pwm = desired_pwm - current_pwm;

        float elapsed_sec = update_period_ms / 1000.0f;
        float pwm_step = shared::util::Clamper<float>::Evaluate(
            delta_pwm, -pwm_roc_ * elapsed_sec, pwm_roc_ * elapsed_sec);

        pwm_.SetDutyCycle(current_pwm + pwm_step);
    }

private:
    shared::periph::PWMOutput& pwm_;

    /// @brief Mapping from temperature [degC] to fan power
    /// Spin the fan faster when the acculumator is hotter.
    static constexpr auto temp_to_power_ =
        std::to_array<shared::util::LookupTable<float>::Entry>({
            {19, 0},
            {20, 30},
            {50, 100},
        });

    /// @brief Maximum PWM rate of change in %/second.
    const float pwm_roc_ = 20.0f;
};

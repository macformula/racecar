/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#pragma once

#include <array>

#include "lookup_table.hpp"
#include "periph/pwm.hpp"

class FanController {
public:
    FanController(macfe::periph::PWMOutput& pwm);

    void Start(float initial_power);
    void Update(float temperature, float update_period_ms);

private:
    macfe::periph::PWMOutput& pwm_;

    /// @brief Mapping from temperature [degC] to fan power
    /// Spin the fan faster when the acculumator is hotter.
    static constexpr auto temp_to_power_ =
        std::to_array<macfe::LookupTable::Entry>({
            {19, 0},
            {20, 30},
            {50, 100},
        });

    /// @brief Maximum PWM rate of change in %/second.
    const float pwm_roc_ = 20.0f;
};

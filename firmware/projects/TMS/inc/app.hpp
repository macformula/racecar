/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"

/***************************************************************
    App-level objects
***************************************************************/

class TempSensor {
public:
    TempSensor(shared::periph::AnalogInput& analog_input,
               const shared::util::Mapper<float>& volt_to_temp)
        : analog_input_(analog_input),
          volt_to_temp_(volt_to_temp),
          rolling_temperature_() {}

    float Update() {
        float new_temperature = Read();
        rolling_temperature_.LoadValue(new_temperature);
        return rolling_temperature_.GetValue();
    }

private:
    shared::periph::AnalogInput& analog_input_;

    /// @brief Mapping from raw ADC value to temperature [degC]
    const shared::util::Mapper<float>& volt_to_temp_;

    static constexpr int moving_average_length_ = 20;
    shared::util::MovingAverage<float, moving_average_length_>
        rolling_temperature_;

    float Read() {
        float volt = analog_input_.ReadVoltage();
        float temperature = volt_to_temp_.Evaluate(volt);
        return temperature;
    }
};

class FanContoller {
public:
    FanContoller(shared::periph::PWMOutput& pwm,
                 shared::util::Mapper<float>& temp_to_power)
        : pwm_(pwm), temp_to_power_(temp_to_power) {}

    void Start(float initial_power) {
        pwm_.Start();
        pwm_.SetDutyCycle(power_to_pwm_.Evaluate(initial_power));
    }

    void Update(float temperature, float update_period_ms) {
        // convert pwm = 100 - power since the fan runs on inverse logic
        // ex. pwm=20% => fan is running at 80%
        float desired_pwm =
            power_to_pwm_.Evaluate(temp_to_power_.Evaluate(temperature));
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
    shared::util::Mapper<float>& temp_to_power_;

    /// @brief Fan pwm signal is inverted (high duty = low power)
    const shared::util::LinearMap<float> power_to_pwm_{-1.0f, 100.0f};

    /// @brief Maximum PWM rate of change in %/second.
    const float pwm_roc_ = 20.0f;
};

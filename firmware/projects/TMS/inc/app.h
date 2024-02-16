/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"
#include "shared/util/mappers/mapper.h"
#include "shared/util/moving_average.h"

/***************************************************************
    App-level objects
***************************************************************/

class TempSensor {
public:
    TempSensor(shared::periph::ADCInput& adc,
               shared::util::Mapper<float>& adc_to_temp)
        : adc_(adc), adc_to_temp_(adc_to_temp), rolling_temperature_() {}

private:
    shared::periph::ADCInput& adc_;

    /// @brief Mapping from raw ADC value to temperature [degC]
    shared::util::Mapper<float>& adc_to_temp_;

    shared::util::MovingAverage<float, 20> rolling_temperature_;

    float Read() {
        uint32_t adc_value = adc_.Read();
        float temperature = adc_to_temp_.Evaluate(float(adc_value));
        return temperature;
    }

    float Update() {
        float new_temperature = Read();
        rolling_temperature_.LoadValue(new_temperature);
        return GetTemperature();
    }

    float GetTemperature() {
        return rolling_temperature_.GetValue();
    }

    template <int i>
    friend class TempSensorManager;
};

template <int sensor_count_>
class TempSensorManager {
public:
    TempSensorManager(TempSensor* sensors) : sensors_(sensors) {}

    /// @brief Updates the temperature values from each temperature sensor.
    void Update() {
        for (int i = 0; i < sensor_count_; i++) {
            temperatures_[i] = sensors_[i].Update();
        }
    }

    /// @brief Copy temperature values to another buffer.
    /// @param buffer
    void GetTemperatures(float* buffer) {
        for (int i = 0; i < sensor_count_; i++) {
            buffer[i] = temperatures_[i];
        }
    }

private:
    TempSensor* sensors_;
    float temperatures_[sensor_count_];
};

class FanContoller {
public:
    FanContoller(shared::periph::PWMOutput& pwm,
                 shared::util::Mapper<float>& temp_to_pwm, float pwm_step_size)
        : pwm_(pwm), temp_to_pwm_(temp_to_pwm), pwm_step_size_(pwm_step_size) {}

    void Update(float temperature) {
        static float desired_pwm = temp_to_pwm_.Evaluate(temperature);
        float current_pwm = pwm_.GetDutyCycle();
        float delta_pwm = desired_pwm - current_pwm;

        float pwm_step = shared::util::Clamper<float>::Evaluate(
            delta_pwm, -pwm_step_size_, pwm_step_size_);

        pwm_.SetDutyCycle(current_pwm + pwm_step);
    }

    void StartPWM(float initial_duty_cycle) {
        pwm_.Start();
        pwm_.SetDutyCycle(initial_duty_cycle);
    }

private:
    shared::periph::PWMOutput& pwm_;

    /// @brief Mapping from temperature [degC] to fan PWM
    shared::util::Mapper<float>& temp_to_pwm_;

    /// @brief Largest allowable PWM per Update() call.
    /// @todo Express pwm_step_size in pwm/second and use Update() frequency to
    /// determine it.
    float pwm_step_size_;
};

class DebugIndicator {
private:
    shared::periph::DigitalOutput& digital_output_;

public:
    DebugIndicator(shared::periph::DigitalOutput& digital_output)
        : digital_output_(digital_output) {}

    void Set() {
        digital_output_.SetHigh();
    }

    void Reset() {
        digital_output_.SetLow();
    }
};
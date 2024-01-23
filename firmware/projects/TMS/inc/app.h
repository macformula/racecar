/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#ifndef TMS_APP_H_
#define TMS_APP_H_

#include <cstdint>
#include <string>

#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/data_structures/lookup_table.h"

/***************************************************************
    App-level objects
***************************************************************/

template <shared::periph::ADCInput ADCInput>
class TempSensor {
    using LUT = shared::util::LookupTable;

private:
    ADCInput& adc_;

    /// @brief Mapping from raw ADC value to temperature [degC]
    LUT& adc_to_temp_;

public:
    TempSensor(ADCInput& adc, LUT& adc_to_temp)
        : adc_(adc), adc_to_temp_(adc_to_temp) {}

    float Read() {
        uint32_t adc_value = adc_.Read();
        float temperature = adc_to_temp_.Interpolate(float(adc_value));
        return temperature;
    }
};

template <class T>
class TempSensorManager {
public:
    TempSensorManager(TempSensor sensors[], int sensor_count)
        : sensors_(sensors), sensor_count_(sensor_count) {}

private:
    TempSensor sensors_[];
    int sensor_count_;
};

template <shared::periph::PWMOutput PWMOutput>
class FanContoller {
    using LUT = shared::util::LookupTable;

private:
    PWMOutput& pwm_;

    /// @brief Mapping from temperature [degC] to fan PWM
    LUT& temp_to_pwm_;

public:
    FanContoller(PWMOutput& pwm, LUT& temp_to_pwm)
        : pwm_(pwm), temp_to_pwm_(temp_to_pwm) {}

    void Update(float temperature) {
        float new_pwm = temp_to_pwm_.Interpolate(temperature);
        pwm_.SetDutyCycle(new_pwm);
    }

    void StartPWM() {
        pwm_.Start();
    }
};

template <shared::periph::DigitalOutput DigitalOutput>
class DebugIndicator {
private:
    DigitalOutput& digital_output_;

public:
    DebugIndicator(DigitalOutput& digital_output)
        : digital_output_(digital_output) {}

    void Set() {
        digital_output_.SetHigh();
    }

    void Reset() {
        digital_output_.SetLow();
    }
};

#endif
/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#ifndef TMS_APP_H_
#define TMS_APP_H_

#include <cstddef>
#include <cstdint>
#include <string>

#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/mapper.h"

/***************************************************************
    App-level objects
***************************************************************/

class TempSensor {
private:
    shared::periph::ADCInput& adc_;

    /// @brief Mapping from raw ADC value to temperature [degC]
    shared::util::Mapper& adc_to_temp_;

public:
    TempSensor(shared::periph::ADCInput& adc, shared::util::Mapper& adc_to_temp)
        : adc_(adc), adc_to_temp_(adc_to_temp) {}

    float Read() {
        uint32_t adc_value = adc_.Read();
        float temperature = adc_to_temp_.Evaluate(float(adc_value));
        return temperature;
    }
};

template <int sensor_count_>
class TempSensorManager {
public:
    TempSensorManager(TempSensor* sensors) : sensors_(sensors) {}

    void ReadSensors(uint32_t* buffer) {
        for (int i = 0; i < sensor_count_; i++) {
            buffer[i] = sensors_[i].Read();
        }
    }

private:
    TempSensor* sensors_;
};

class FanContoller {
private:
    shared::periph::PWMOutput& pwm_;

    /// @brief Mapping from temperature [degC] to fan PWM
    shared::util::Mapper& temp_to_pwm_;

public:
    FanContoller(shared::periph::PWMOutput& pwm,
                 shared::util::Mapper& temp_to_pwm)
        : pwm_(pwm), temp_to_pwm_(temp_to_pwm) {}

    void Update(float temperature) {
        float new_pwm = temp_to_pwm_.Evaluate(temperature);
        pwm_.SetDutyCycle(new_pwm);
    }

    void StartPWM() {
        pwm_.Start();
    }
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

#endif
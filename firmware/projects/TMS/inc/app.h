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
    Function definititions (for those that need an mcal binding)
***************************************************************/

void Initialize();
void Log(std::string message);

/***************************************************************
    App-level objects
***************************************************************/

template <shared::periph::ADCInput ADCInput>
class TempSensor {
    using LUT = shared::util::LookupTable;

private:
    ADCInput& adc_;
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

template <shared::periph::PWMOutput PWMOutput>
class FanContoller {
private:
    PWMOutput& pwm_;

public:
    FanContoller(PWMOutput& pwm) : pwm_(pwm) {}

    void Set(float value) {
        pwm_.SetDutyCycle(value);
    }

    void StartPWM() {
        pwm_.Start();
    }
};

#endif
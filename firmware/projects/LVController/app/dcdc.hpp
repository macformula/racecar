#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/gpio.hpp"

class DCDC {
public:
    DCDC(shared::periph::DigitalOutput& enable_output_inverted,
         shared::periph::DigitalOutput& select_sense,
         shared::periph::AnalogInput& sense);

    void Enable();
    void Disable();

    float MeasureVoltage();
    float MeasureAmps();

private:
    shared::periph::DigitalOutput& enable_output_inverted_;
    shared::periph::DigitalOutput& select_sense_;
    shared::periph::AnalogInput& sense_;
};
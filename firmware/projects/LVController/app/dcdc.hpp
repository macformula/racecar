#pragma once

#include "shared/periph/gpio.hpp"
#include "subsystem.hpp"


class DCDC : public Subsystem {
public:
    DCDC(shared::periph::DigitalOutput& enable_output,
         shared::periph::DigitalInput& valid_input,
         shared::periph::DigitalOutput& led_output);

    bool CheckValid();

private:
    shared::periph::DigitalInput& valid_input_;
    shared::periph::DigitalOutput& led_;
};
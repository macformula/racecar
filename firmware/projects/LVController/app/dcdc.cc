#include "dcdc.hpp"

#include "shared/periph/gpio.hpp"


DCDC::DCDC(shared::periph::DigitalOutput& enable_output,
           shared::periph::DigitalInput& valid_input,
           shared::periph::DigitalOutput& led_output)
    : Subsystem(enable_output), valid_input_(valid_input), led_(led_output){};

bool DCDC::CheckValid() {
    bool is_valid = valid_input_.Read();
    led_.Set(is_valid);
    return is_valid;
}
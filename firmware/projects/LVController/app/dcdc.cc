#include "dcdc.hpp"

#include "shared/periph/gpio.hpp"

DCDC::DCDC(shared::periph::DigitalOutput& enable_output,
           shared::periph::DigitalInput& valid_input)
    : Subsystem(enable_output), valid_input_(valid_input){};

bool DCDC::CheckValid() {
    bool is_valid = valid_input_.Read();
    return is_valid;
}
#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/analog_output.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "mcal/cli/periph/pwm.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

namespace mcal {
using namespace mcal::cli::periph;

DigitalInput button_increment{"button_increment"};
DigitalInput button_decrement{"button_decrement"};
DigitalInput button_confirm{"button_confirm"};

DigitalOutput led_position0{"led_position0"};
DigitalOutput led_position1{"led_position1"};

PWMOutput pwm_output0{"pwm_output0"};

AnalogOutput analog_output0{"analog_output0"};
AnalogOutput analog_output1{"analog_output1"};
AnalogOutput analog_output2{"analog_output2"};

AnalogOutput analog_outputs[bindings::kNumAnalogOutputs] = {
    analog_output0, analog_output1, analog_output2};

AnalogOutputGroup<bindings::kNumAnalogOutputs> analog_output_group(
    "analog_output_group", analog_outputs);

}  // namespace mcal

namespace bindings {

shared::periph::DigitalInput& button_increment = mcal::button_increment;
shared::periph::DigitalInput& button_decrement = mcal::button_decrement;
shared::periph::DigitalInput& button_confirm = mcal::button_confirm;

shared::periph::DigitalOutput& led_position0 = mcal::led_position0;
shared::periph::DigitalOutput& led_position1 = mcal::led_position1;

shared::periph::PWMOutput& pwm_output0 = mcal::pwm_output0;

shared::periph::AnalogOutputGroup<kNumAnalogOutputs>& analog_output_group =
    mcal::analog_output_group;

void DelayMs(uint32_t ms) {
    usleep(ms * 1000);
}

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings
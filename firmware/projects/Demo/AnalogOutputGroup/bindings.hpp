#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"
namespace bindings {

constexpr uint8_t kNumAnalogOutputs = 3;

extern shared::periph::DigitalInput& button_increment;
extern shared::periph::DigitalInput& button_decrement;
extern shared::periph::DigitalInput& button_confirm;

extern shared::periph::DigitalOutput& led_position0;
extern shared::periph::DigitalOutput& led_position1;

extern shared::periph::PWMOutput& pwm_output0;

extern shared::periph::AnalogOutputGroup<kNumAnalogOutputs>&
    analog_output_group;

extern void DelayMs(uint32_t ms);
extern void Init();

}  // namespace bindings
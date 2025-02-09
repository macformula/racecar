#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

namespace bindings {

constexpr uint8_t kNumChannelsPerDac = 4;
constexpr uint8_t kNumExternalDacs = 2;
constexpr uint8_t kNumAnalogOutputs = kNumChannelsPerDac * kNumExternalDacs;
constexpr float kVoltageReference = 3.3F;

extern shared::periph::DigitalInput& button_increment;
extern shared::periph::DigitalInput& button_decrement;
extern shared::periph::DigitalInput& button_confirm;

extern shared::periph::DigitalOutput& led_position0;
extern shared::periph::DigitalOutput& led_position1;
extern shared::periph::DigitalOutput& led_position2;

extern shared::periph::PWMOutput& pwm_output0;

extern shared::periph::AnalogOutputGroup<kNumAnalogOutputs>&
    analog_output_group;

extern void DelayMs(uint32_t ms);
extern void Init();

}  // namespace bindings
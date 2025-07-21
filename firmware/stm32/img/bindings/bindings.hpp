// This file is for illustrative purposes only and does not indicate the
// complete or correct LV Controller code.

#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalOutput& tssi_red_en;
extern shared::periph::DigitalOutput& tssi_green_en;

extern shared::periph::AnalogInput& dcdc_sense;
extern shared::periph::AnalogInput& suspension_travel3;
extern shared::periph::AnalogInput& suspension_travel4;

extern shared::periph::CanBase& veh_can_base;

// Note: JTAG isn't used in any of our code, but the CubeMX pin configuration is
// still necessary.

extern void Initialize();

}  // namespace bindings
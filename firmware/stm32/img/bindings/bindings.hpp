// This file is for illustrative purposes only and does not indicate the
// complete or correct LV Controller code.

#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern macfe::periph::DigitalOutput& tssi_red_en;
extern macfe::periph::DigitalOutput& tssi_green_en;

extern macfe::periph::AnalogInput& dcdc_sense;
extern macfe::periph::AnalogInput& suspension_travel3;
extern macfe::periph::AnalogInput& suspension_travel4;

extern macfe::periph::CanBase& veh_can_base;

// Note: JTAG isn't used in any of our code, but the CubeMX pin configuration is
// still necessary.

extern void Initialize();

}  // namespace bindings
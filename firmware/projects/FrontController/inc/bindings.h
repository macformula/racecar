/// @author Blake Freer
/// @date 2024-05-29

#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"

namespace bindings {

extern shared::periph::DigitalOutput&& driver_speaker;
extern shared::periph::DigitalOutput&& brake_light;
extern shared::periph::ADCInput&& accel_pedal_1;
extern shared::periph::ADCInput&& accel_pedal_2;
extern shared::periph::ADCInput&& brake_pedal;
extern shared::periph::ADCInput&& steering_wheel;
extern shared::periph::DigitalInput&& driver_button;

extern void Initialize();
}  // namespace bindings
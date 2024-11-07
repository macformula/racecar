#include <unistd.h>

#include "bindings.h"
#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"

// fw imports
#include "mcal/cli/periph/adc.h"
#include "mcal/cli/periph/can.h"
#include "mcal/cli/periph/gpio.h"

namespace mcal {
using namespace cli::periph;

ADCInput accel_pedal_1{"apps1"};
ADCInput accel_pedal_2{"apps2"};

CanBase vehicle_can_base{"vehicle"};

DigitalOutput debug_led{"Debug LED"};
}  // namespace mcal

namespace bindings {
shared::periph::ADCInput& accel_pedal_1 = mcal::accel_pedal_1;
shared::periph::ADCInput& accel_pedal_2 = mcal::accel_pedal_2;
shared::periph::CanBase& vehicle_can_base = mcal::vehicle_can_base;

shared::periph::DigitalOutput& debug_led = mcal::debug_led;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
    mcal::vehicle_can_base.Setup();
}

void DelayMS(uint32_t ms) {
    usleep(ms * 1000);
}

}  // namespace bindings
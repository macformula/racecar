#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include <unistd.h>

#include "bindings.h"

// fw imports
#include "mcal/cli/periph/adc.h"
#include "mcal/cli/periph/can.h"

namespace mcal {
using namespace cli::periph;

ADCInput accel_pedal_1{
    "apps1"
};
ADCInput accel_pedal_2{
    "apps2"
};

CanBase vehicle_can_base{
    "vehicle"
};
}

namespace bindings {
shared::periph::ADCInput& accel_pedal_1 = mcal::accel_pedal_1;
shared::periph::ADCInput& accel_pedal_2 = mcal::accel_pedal_2;
shared::periph::CanBase& vehicle_can_base = mcal::vehicle_can_base;

void Initialize() {


}

void DelayMS(uint32_t ms) {
    usleep(ms * 1000);
}

}  // namespace bindings
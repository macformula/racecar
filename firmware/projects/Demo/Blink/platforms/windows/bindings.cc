#include <unistd.h>

#include "bindings.h"
#include "mcal/windows/periph/gpio.h"
#include "shared/periph/gpio.h"


namespace mcal {
using namespace windows;
periph::DigitalOutput indicator{"Indicator"};
}  // namespace mcal

namespace bindings {
shared::periph::DigitalOutput& indicator = mcal::indicator;

void DelayMS(uint32_t ms) {
    usleep(ms * 1000);
}
}  // namespace bindings
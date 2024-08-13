#include <cstdint>

#include "shared/periph/gpio.h"


namespace bindings {
extern shared::periph::DigitalOutput& indicator;

extern void DelayMS(uint32_t ms);
}  // namespace bindings
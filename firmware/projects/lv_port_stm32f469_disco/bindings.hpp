#include <cstdint>

#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::CanBase& veh_can_base;
extern shared::periph::DigitalInput& button_scroll;
extern shared::periph::DigitalInput& button_select;

extern void Initialize();

extern void DelayMS(uint32_t ms);

}  // namespace bindings
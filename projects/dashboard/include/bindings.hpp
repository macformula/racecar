#include <cstdint>

#include "periph/can.hpp"
#include "periph/gpio.hpp"

namespace bindings {

extern macfe::periph::CanBase& veh_can_base;
extern macfe::periph::DigitalInput& button_scroll;
extern macfe::periph::DigitalInput& button_enter;

extern void Initialize();

extern void Shutdown();

extern void DelayMS(uint32_t ms);

extern bool ShouldQuit();

}  // namespace bindings
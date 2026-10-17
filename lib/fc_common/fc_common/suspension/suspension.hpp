#pragma once

#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"

namespace suspension {

void ReadAndSendTravel(generated::can::VehBus& veh_can,
                       macfe::periph::AnalogInput& suspension_travel1,
                       macfe::periph::AnalogInput& suspension_travel2);

}  // namespace suspension

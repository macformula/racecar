#pragma once

#include "generated/can/veh_bus.hpp"
#include "periph/gpio.hpp"

namespace brake_light {
void task_100hz(generated::can::VehBus& veh_can,
                macfe::periph::DigitalOutput& brake_light_en);

}  // namespace brake_light
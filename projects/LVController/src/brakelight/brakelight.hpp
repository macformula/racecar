#pragma once

#include "generated/can/veh_bus.hpp"

namespace brake_light {

void task_100hz(generated::can::VehBus& veh_can);

}  // namespace brake_light
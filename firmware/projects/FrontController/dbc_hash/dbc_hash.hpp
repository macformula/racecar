#pragma once

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"

namespace dbc_hash {

bool IsValid(void);
void Update_10Hz(generated::can::VehBus& veh_can);

}  // namespace dbc_hash
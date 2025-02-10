#pragma once

#include "shared/device/digital_pot.hpp"
#include "shared/device/mcp/mcp4461.hpp"
#include "shared/periph/i2c.hpp"

namespace device {

extern shared::device::DigitalPotentiometer<uint8_t> digital_pot0;
extern shared::device::DigitalPotentiometer<uint8_t> digital_pot1;

}  // namespace devices

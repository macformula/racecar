#include "bindings.hpp"
#include "devices.hpp"
#include "shared/device/mcp/mcp4461.hpp"
#include "shared/periph/i2c.hpp"

namespace device {

// Anonymous namespace used to limit the scope of these variables to this file
// only. This prevents name collisions with similar variables in other
// translation units.
namespace {
constexpr uint8_t kMCP4461Addr = 0x2C;
shared::device::mcp::MCP4461 digital_pot_controller(bindings::i2c_bus1,
                                                    kMCP4461Addr);
}  // namespace

shared::device::DigitalPotentiometer<uint8_t> digital_pot0(
    digital_pot_controller, 0);
shared::device::DigitalPotentiometer<uint8_t> digital_pot1(
    digital_pot_controller, 1);

}  // namespace device

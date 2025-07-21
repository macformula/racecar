#include "dbc_hash.hpp"

namespace dbc_hash {

static bool valid = false;

bool IsValid(void) {
    return valid;
}

void Update_10Hz(generated::can::VehBus& veh_can) {
    auto msg = veh_can.GetRxLvDbcHash();

    if (msg.has_value()) {
        valid = (msg->Hash() == generated::can::kVehDbcHash);
    }
}

}  // namespace dbc_hash
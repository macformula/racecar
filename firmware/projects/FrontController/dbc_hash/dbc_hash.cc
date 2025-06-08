#include "dbc_hash.hpp"

namespace dbc_hash {

static bool valid = false;

bool IsValid(void) {
    return valid;
}

void Update_10Hz(generated::can::VehBus& veh_can) {
    auto msg = veh_can.GetRxSyncDbcHash();

    if (msg.has_value()) {
        valid = (msg->DbcHash() == generated::can::kVehDbcHash);
    }
}

}  // namespace dbc_hash
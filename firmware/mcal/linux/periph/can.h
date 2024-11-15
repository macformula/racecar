#pragma once

#include <thread>

#include "shared/comms/can/msg.h"
#include "shared/periph/can.h"
#include "vcan/vcan.h"

namespace mcal::lnx::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface);

    void Setup();
    void Send(const shared::can::RawMessage&) override;

private:
    uint32_t GetTimestamp() const override;

    struct vcan::VcanSocket socket_;

    std::thread reader_thread_;
    void StartReading();
};
}  // namespace mcal::lnx::periph
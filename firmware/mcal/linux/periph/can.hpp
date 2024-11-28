#pragma once

#include <mutex>
#include <queue>
#include <thread>

#include "mcal/linux/periph/vcan/vcan.hpp"
#include "shared/comms/can/raw_can_msg.hpp"
#include "shared/periph/can.hpp"
#include "vcan/vcan.h"

namespace mcal::lnx::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface);

    void Setup();
    void Send(const shared::can::RawCanMsg&);
    void ReadQueue(shared::can::RawCanMsg[], size_t len);

private:
    struct vcan::VcanSocket socket_;

    std::queue<shared::can::RawCanMsg> can_queue_;
    std::mutex queue_access_;
    std::thread reader_thread_;

    void StartReading();
};
}  // namespace mcal::lnx::periph
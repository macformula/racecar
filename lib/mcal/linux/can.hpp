#pragma once

#include <thread>

#include "mcal/linux/vcan/vcan.hpp"
#include "periph/can.hpp"
#include "vcan/vcan.hpp"

namespace mcal::lnx {

class CanBase : public macfe::periph::CanBase {
public:
    CanBase(std::string can_iface, bool log_rx = true);

    void Setup();
    void Send(const macfe::can::RawMessage&) override;

private:
    uint32_t GetTimestamp() const override;

    struct vcan::VcanSocket socket_;
    bool log_rx_;

    std::thread reader_thread_;
    void StartReading();
};
}  // namespace mcal::lnx
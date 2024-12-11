/// @author Samuel Parent
/// @date 2024-04-10

#pragma once

#include <string.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <format>
#include <iostream>

#include "shared/comms/can/msg.hpp"
#include "shared/periph/can.hpp"

namespace mcal::cli::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface){};

    void Setup() {
        std::cout << std::format("can interface: {}", iface_) << std::endl;
    }

    void Send(const shared::can::RawMessage& msg) override {
        std::cout << std::format("CanBase {}: Sending\n| {}", iface_, msg)
                  << std::endl;
    }

private:
    std::string iface_;

    uint32_t GetTimestamp() const override {
        using namespace std::chrono;
        auto t = system_clock::now().time_since_epoch();
        auto ms = duration_cast<milliseconds>(t).count();
        return ms;
    }
};

}  // namespace mcal::cli::periph

#pragma once

#include <unistd.h>

#include <chrono>
#include <cstring>
#include <format>
#include <iostream>

#include "can/msg.hpp"
#include "periph/can.hpp"

namespace mcal::sil {

class CanBase : public macfe::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface) {};

    void Setup() {
        std::cout << std::format("can interface: {}", iface_) << std::endl;
    }

    void Send(const macfe::can::RawMessage& msg) override {
        // std::cout << std::format("CanBase {}: Sending\n| {}", iface_, msg)
        //           << std::endl;
    }

private:
    std::string iface_;

    uint32_t GetTimestamp() const override {
        using namespace std::chrono;
        // auto t = system_clock::now().time_since_epoch();
        // auto ms = duration_cast<milliseconds>(4).count();
        return 4;
    }
};
}  // namespace mcal::sil
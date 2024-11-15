/// @author Samuel Parent
/// @date 2024-04-10

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cstring>
#include <iomanip>
#include <iostream>

#include "shared/comms/can/msg.h"
#include "shared/periph/can.h"

namespace mcal::cli::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface) {};

    void Setup() {
        std::cout << "can interface: " << iface_ << std::endl;
    }

    void Send(const shared::can::RawMessage& msg) {
        std::cout << std::format("{} {}", iface_, msg) << std::endl;
    }

private:
    std::string iface_;

    uint32_t GetTimestamp() const override {
        return 0;
    }
};

}  // namespace mcal::cli::periph

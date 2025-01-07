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
#include <format>

#include "shared/comms/can/raw_can_msg.hpp"
#include "shared/periph/can.hpp"

namespace mcal::cli::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface) {};

    void Setup() {
        std::cout << std::format("can interface: {}", iface_) << std::endl;
    }

    void Send(const shared::can::RawCanMsg& can_tx_msg) {
        std::cout << std::format("{} [{:02X}] ", iface_, can_tx_msg.header.id) << std::endl;

        // Loop through each data byte and print it in uppercase hex with
        // leading zeros
        for (int i = 0; i < sizeof(can_tx_msg.data); ++i) {
            std::cout << std::format("{:02X} ", can_tx_msg.data[i]); 
        }
        std::cout << std::endl;
    }

    void ReadQueue(shared::can::RawCanMsg can_rx_msgs[], size_t len) {}

private:
    using RawCanMsg = shared::can::RawCanMsg;
    std::string iface_;
};

}  // namespace mcal::cli::periph

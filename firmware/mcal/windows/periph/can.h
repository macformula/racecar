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

#include "shared/comms/can/raw_can_msg.h"
#include "shared/periph/can.h"

namespace mcal::windows::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface){};

    void Setup() {
        std::cout << "can interface: " << iface_ << std::endl;
    }

    void Send(const shared::can::RawCanMsg& can_tx_msg) {
        std::cout << iface_ << " [" << std::hex << std::uppercase
                  << std::setfill('0') << std::setw(2) << can_tx_msg.header.id
                  << "] ";

        // Loop through each data byte and print it in uppercase hex with
        // leading zeros
        for (int i = 0; i < sizeof(can_tx_msg.data); ++i) {
            std::cout << std::hex << std::uppercase << std::setfill('0')
                      << std::setw(2) << static_cast<int>(can_tx_msg.data[i])
                      << " ";
        }

        std::cout << std::endl;
    }

    void ReadQueue(shared::can::RawCanMsg can_rx_msgs[], size_t len) {}

private:
    using RawCanMsg = shared::can::RawCanMsg;
    std::string iface_;
};

}  // namespace mcal::windows::periph

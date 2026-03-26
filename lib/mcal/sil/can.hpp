#pragma once

#include <fmt/core.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iostream>

#include "can/msg.hpp"
#include "periph/can.hpp"

namespace mcal::sil {

class CanBase : public macfe::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface) {};

    void Setup() {
        // Create a socket
        sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (sock_ < 0) {
            perror("Error creating socket");
            return;
        }

        // Specify the can interface
        strncpy(ifreq_.ifr_name, iface_.c_str(), sizeof(ifreq_.ifr_name) - 1);

        std::cout << fmt::format("can interface: {}", iface_) << std::endl;

        ioctl(sock_, SIOCGIFINDEX, &ifreq_);

        // Bind the socket to the can interface
        sock_addr_.can_family = AF_CAN;
        sock_addr_.can_ifindex = ifreq_.ifr_ifindex;

        int bind_status =
            bind(sock_, (struct sockaddr*)&sock_addr_, sizeof(sock_addr_));
        if (bind_status < 0) {
            perror("Error binding socket");
            close(sock_);
        }

        reader_thread_ = std::thread(&CanBase::StartReading, this);
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
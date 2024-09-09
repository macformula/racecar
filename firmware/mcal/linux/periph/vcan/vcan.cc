#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "vcan.h"

namespace mcal::lnx::periph::vcan {

VcanSocket::VcanSocket(std::string iface) : iface_(iface) {}

int VcanSocket::Open() {
    socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_ == -1) {
        perror("Error while opening socket.");
        return -1;
    }
    strcpy(ifr_.ifr_name, iface_.c_str());
    ioctl(socket_, SIOGIFINDEX, &ifr_);

    addr_.can_family = AF_CAN;
    addr_.can_ifindex = ifr_.ifr_ifindex;

    int bind_status = bind(socket_, (struct sockaddr*)&addr_, sizeof(addr_));
    if (bind_status == -1) {
        perror("Error in socket bind.");
        return -1;
    }

    std::cout << "Opened " << iface_ << " at index " << addr_.can_ifindex
              << std::endl;

    return 0;
}

VcanSocket::~VcanSocket() {
    std::cout << "closing" << std::endl;
    close(socket_);
}

int VcanSocket::Write(struct can_frame* frame) {
    return write(socket_, frame, sizeof(struct can_frame));
}

int VcanSocket::Read(struct can_frame* frame) {
    return read(socket_, frame, sizeof(struct can_frame));
}

}  // namespace mcal::lnx::periph::vcan
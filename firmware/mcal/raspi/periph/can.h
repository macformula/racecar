/// @author Samuel Parent
/// @date 2023-01-12

#ifndef MCAL_RASPI_PERIPH_CAN_H_
#define MCAL_RASPI_PERIPH_CAN_H_

#include <cstdint>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "shared/comms/can/raw_can_msg.h"
#include "shared/util/peripheral.h"

namespace mcal::periph {

class CanBase : public shared::util::Peripheral {

private:
    struct sockaddr_can sock_addr_;
    struct ifreq ifreq_;
    struct can_frame frame_;
    std::string iface_;
    int sock_;

public:
	CanBase(std::string can_iface) : iface_(can_iface) {};

    void Setup() {
        // Create a socket
        if ((sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            perror("Error creating socket");
            return;
        }

        // Specify the can interface
        strncpy(ifreq_.ifr_name, iface_.c_str(), sizeof(ifreq_.ifr_name) - 1);

        std::cout << "can interface: " << iface_ << std::endl;
        
        ioctl(sock_, SIOCGIFINDEX, &ifreq_);

        // Bind the socket to the can interface
        sock_addr_.can_family = AF_CAN;
        sock_addr_.can_ifindex = ifreq_.ifr_ifindex;
        if (bind(sock_, (struct sockaddr *)&sock_addr_, sizeof(sock_addr_)) < 0) {
            perror("Error binding socket");
            close(sock_);
        }

        return;
    }
	void Send(const shared::comms::can::raw_can_msg& can_tx_msg) {
        frame_.can_id = can_tx_msg.can_hdr.can_id;
        frame_.can_dlc = can_tx_msg.can_hdr.data_len;
        memcpy((uint8_t *)frame_.data, can_tx_msg.bytes, 8);

        if (write(sock_, &frame_, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Error writing to socket");
            close(sock_);
        }            
        
        return;
    }
    void ReadQueue(shared::comms::can::raw_can_msg can_rx_msgs[]) {
        return;
    }
};

} // namespace mcal::periph

#endif
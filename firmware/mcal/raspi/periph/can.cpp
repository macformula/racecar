#include "mcal/raspi/periph/can.h"
#include "shared/comms/can/raw_can_msg.h"

#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

namespace mcal::periph {

CanBase::Setup() {
    // Create a socket
    if ((sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Error creating socket");
        return;
    }

    // Specify the can0 interface
    strcpy(ifreq_.ifr_name, iface_);
    ioctl(sock_, SIOCGIFINDEX, &ifreq_);

    // Bind the socket to the can interface
    sock_addr_.can_family = AF_CAN;
    sock_addr_.can_ifindex = ifreq_.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&sock_addr_, sizeof(sock_addr_)) < 0) {
        perror("Error binding socket");
        close(s);
    }

    return;
}

CanBase::Send(const shared::comms::can::raw_can_msg& can_tx_msg) {
    frame_.can_id = can_tx_msg.can_hdr.can_id;
    frame_.can_dlc = can_tx_msg.can_hdr.data_len;
    memccpy((uint8_t *)frame.data, can_tx_msg.data, 8);

    if (write(sock_, &frame_, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Error writing to socket");
        close(sock_);
    }            
    
    return;
}                

CanBase::ReadQueue(shared::comms::can::raw_can_msg can_rx_msgs[]) {
    return;
}

} // namespace mcal::periph

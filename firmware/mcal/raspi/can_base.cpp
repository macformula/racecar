#include "shared/periph/can.h"
#include "shared/comms/can/raw_can_msg.h"
#include "shared/comms/can/can_msg.h"
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

void Transmit(raw_can_msg& can_tx_msg) {
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // Create a socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Specify the can0 interface
    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    // Bind the socket to the can0 interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(s);
    }

    // Set up a CAN frame to send
    frame.can_id = 0x123; // CAN ID
    frame.can_dlc = 8;    // Data Length Code
    strcpy((char *)frame.data, can_tx_msg);

    // Send the CAN frame
    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Error writing to socket");
        close(s);
        return 1;
    }
    return;
}
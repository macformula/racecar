/// @author Samuel Parent
/// @date 2023-01-12

#ifndef MCAL_RASPI_PERIPH_CAN_H_
#define MCAL_RASPI_PERIPH_CAN_H_

#include <cstdint>
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

    Setup();
	Send(const shared::comms::can::raw_can_msg& can_tx_msg);
    ReadQueue(shared::comms::can::raw_can_msg can_rx_msgs[]);
};

} // namespace mcal::periph

#endif
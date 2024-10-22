#pragma once

#include <linux/can.h>
#include <net/if.h>
#include <sys/socket.h>

#include <string>

namespace mcal::lnx::periph::vcan {

class VcanSocket {
public:
    VcanSocket(std::string iface);
    ~VcanSocket();

    int Open();
    int Write(struct can_frame* frame);
    int Read(struct can_frame* frame);
    std::string GetIface() const;

private:
    std::string iface_;
    int socket_;
    struct sockaddr_can addr_;
    struct ifreq ifr_;
};

}  // namespace mcal::lnx::periph::vcan
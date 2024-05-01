/// @author Samuel Parent
/// @date 2023-01-12

#pragma once

// #include <linux/can.h>
// #include <linux/can/raw.h>
// #include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <queue>
#include <thread>

#include "shared/comms/can/raw_can_msg.h"
#include "shared/periph/can.h"
#include "third-party/etl/include/etl/queue.h"

namespace mcal::raspi::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface) : iface_(can_iface) {
        program_start_ = std::chrono::steady_clock::now();
    };

    void Setup() {
        // Create a socket
        // sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        // if (sock_ < 0) {
        //     perror("Error creating socket");
        //     return;
        // }

        // // Specify the can interface
        // strncpy(ifreq_.ifr_name, iface_.c_str(), sizeof(ifreq_.ifr_name) - 1);

        // std::cout << "can interface: " << iface_ << std::endl;

        // ioctl(sock_, SIOCGIFINDEX, &ifreq_);

        // // Bind the socket to the can interface
        // sock_addr_.can_family = AF_CAN;
        // sock_addr_.can_ifindex = ifreq_.ifr_ifindex;

        // int bind_status =
        //     bind(sock_, (struct sockaddr*)&sock_addr_, sizeof(sock_addr_));
        // if (bind_status < 0) {
        //     perror("Error binding socket");
        //     close(sock_);
        // }

        // reader_thread_ = std::thread(&CanBase::StartReading, this);
    }

    void Send(const shared::can::RawCanMsg& can_tx_msg) {
        // frame_.can_id = can_tx_msg.header.id;
        // frame_.can_dlc = can_tx_msg.header.data_len;
        // memcpy((uint8_t*)frame_.data, can_tx_msg.data, 8);

        // ssize_t bytes_written = write(sock_, &frame_, sizeof(struct can_frame));
        // if (bytes_written != sizeof(struct can_frame)) {
        //     perror("Error writing to socket");
        //     close(sock_);
        // }
    }

    void ReadQueue(shared::can::RawCanMsg can_rx_msgs[], size_t len) {
        uint16_t msg_idx = 0;
        {
            std::lock_guard<std::mutex> lock(queue_mtx_);
            while (!can_queue_.empty() && (msg_idx < len)) {
                can_rx_msgs[msg_idx] = can_queue_.front();
                can_queue_.pop();
                msg_idx++;
            }
        }
    }

private:
    static constexpr uint8_t kMaxMsgBytes = 8;

    // struct sockaddr_can sock_addr_;
    // struct ifreq ifreq_;
    // struct can_frame frame_;
    std::string iface_;
    int sock_;

    std::queue<shared::can::RawCanMsg> can_queue_;
    std::mutex queue_mtx_;
    std::thread reader_thread_;

    std::chrono::steady_clock::time_point program_start_;

    inline uint32_t get_tick() {
        std::chrono::milliseconds elapsed_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - program_start_);
        return static_cast<uint32_t>(elapsed_ms.count());
    }

    void StartReading() {
        // struct can_frame frame;

        // while (true) {
        //     ssize_t bytesRead =
        //         recv(sock_, &frame, sizeof(struct can_frame), 0);
        //     if (bytesRead == -1) {
        //         perror("Error reading from CAN socket");
        //         continue;
        //     }

        //     shared::can::RawCanMsg rawMsg;
        //     rawMsg.header.id = frame.can_id;
        //     rawMsg.header.data_len = frame.can_dlc;
        //     rawMsg.header.is_extended_frame = frame.can_id & CAN_EFF_FLAG;
        //     rawMsg.tick_timestamp = get_tick();

        //     std::copy(frame.data, frame.data + kMaxMsgBytes, rawMsg.data);

        //     // Add to the queue
        //     {
        //         std::lock_guard<std::mutex> lock(queue_mtx_);
        //         can_queue_.push(rawMsg);
        //     }
        // }
    }
};

}  // namespace mcal::raspi::periph

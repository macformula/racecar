#include "can.h"

#include <linux/can.h>
#include <sys/types.h>

#include <cstring>
#include <format>
#include <iomanip>
#include <mutex>
#include <string>
#include <thread>

#include "shared/comms/can/raw_can_msg.hpp"
#include "vcan/vcan.h"


static std::string message_to_string(const shared::can::RawCanMsg& msg) {
    std::string str = std::format("[{:02X}]", msg.header.id);
    for (int i = 0; i < sizeof(msg.header.data_len); ++i) {
        str += std::format(" {:02X}", msg.data[i]);
    }
    return str;
}

static can_frame to_can_frame(const shared::can::RawCanMsg& msg) {
    struct can_frame frame{.can_id = msg.header.id,
                           .can_dlc = msg.header.data_len};
    std::memcpy(&frame.data, msg.data, 8);
    return frame;
}

static shared::can::RawCanMsg from_can_frame(const can_frame& frame) {
    shared::can::RawCanMsg msg;
    msg.header = {
        .id = frame.can_id,
        .data_len = frame.can_dlc,
        .is_extended_frame = static_cast<bool>(frame.can_id & CAN_EFF_FLAG),
    };
    msg.tick_timestamp = 0;

    std::copy(frame.data, frame.data + 8, msg.data);

    return msg;
}

namespace mcal::lnx::periph {

CanBase::CanBase(std::string iface) : socket_(iface) {}

void CanBase::Setup() {
    socket_.Open();

    // Another thread handles reading
    reader_thread_ = std::thread(&CanBase::StartReading, this);
}

void CanBase::Send(const shared::can::RawCanMsg& can_tx_msg) {
    std::cout << std::format("CanBase {}: Sending\n| {}", socket_.GetIface(),
                             message_to_string(can_tx_msg))
              << std::endl;

    auto frame = to_can_frame(can_tx_msg);
    socket_.Write(&frame);
}

void CanBase::ReadQueue(shared::can::RawCanMsg can_rx_msgs[], size_t len) {
    uint16_t msg_idx = 0;
    std::lock_guard<std::mutex> lock(queue_access_);
    while (!can_queue_.empty() && msg_idx < len) {
        can_rx_msgs[msg_idx] = can_queue_.front();

        std::cout << std::format("CanBase {}: Received\n| {}",
                                 socket_.GetIface(),
                                 message_to_string(can_rx_msgs[msg_idx]))
                  << std::endl;

        can_queue_.pop();
        msg_idx++;
    }
}

void CanBase::StartReading() {
    struct can_frame frame;
    shared::can::RawCanMsg raw_msg;

    while (true) {
        // Block until a frame arrives
        ssize_t bytes_read = socket_.Read(&frame);
        if (bytes_read == -1) {
            perror("Error reading from CAN socket.");
            exit(1);
        }

        raw_msg = from_can_frame(frame);

        {  // push RawCanMsg to queue
            std::lock_guard<std::mutex> lock(queue_access_);
            can_queue_.push(raw_msg);
        }
    }
}

}  // namespace mcal::lnx::periph
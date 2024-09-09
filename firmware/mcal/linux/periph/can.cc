#include <linux/can.h>
#include <sys/types.h>

#include <cstring>
#include <iomanip>
#include <mutex>
#include <string>
#include <thread>

#include "can.h"
#include "shared/comms/can/raw_can_msg.h"
#include "vcan.h"

static void print_msg(std::ostream& stream, const shared::can::RawCanMsg& msg) {
    stream << " [" << std::hex << std::uppercase << std::setfill('0')
           << std::setw(2) << msg.header.id << "]";

    // Loop through each data byte and print it in uppercase hex with leading
    // zeros
    for (int i = 0; i < sizeof(msg.data); ++i) {
        stream << " " << std::hex << std::uppercase << std::setfill('0')
               << std::setw(2) << static_cast<int>(msg.data[i]);
    }
}

namespace mcal::lnx::periph {

CanBase::CanBase(std::string iface) : socket_(iface) {}

void CanBase::Setup() {
    socket_.Open();

    reader_thread_ = std::thread(&CanBase::StartReading, this);
}

void CanBase::Send(const shared::can::RawCanMsg& can_tx_msg) {
    print_msg(std::cout, can_tx_msg);
    std::cout << std::endl;

    struct can_frame frame {
        .can_id = 0x123, .can_dlc = can_tx_msg.header.data_len
    };

    std::memcpy(&frame.data, can_tx_msg.data, 8);

    socket_.Write(&frame);
}

void CanBase::ReadQueue(shared::can::RawCanMsg can_rx_msgs[], size_t len) {
    uint16_t msg_idx = 0;
    {
        std::lock_guard<std::mutex> lock(queue_access_);
        while (!can_queue_.empty() && msg_idx < len) {
            can_rx_msgs[msg_idx] = can_queue_.front();

            print_msg(std::cout, can_rx_msgs[msg_idx]);
            std::cout << std::endl;

            can_queue_.pop();
            msg_idx++;
        }
    }
}

void CanBase::StartReading() {
    struct can_frame frame;
    ssize_t bytes_read;
    shared::can::RawCanMsg raw_msg;

    while (true) {
        bytes_read = socket_.Read(&frame);
        if (bytes_read == -1) {
            perror("Error reading from CAN socket.");
            continue;
        }

        raw_msg.header = {
            .id = frame.can_id,
            .data_len = frame.can_dlc,
            .is_extended_frame = static_cast<bool>(frame.can_id & CAN_EFF_FLAG),
        };
        raw_msg.tick_timestamp = 0;

        std::copy(frame.data, frame.data + 8, raw_msg.data);

        {
            std::lock_guard<std::mutex> lock(queue_access_);
            can_queue_.push(raw_msg);
        }
    }
}

}  // namespace mcal::lnx::periph
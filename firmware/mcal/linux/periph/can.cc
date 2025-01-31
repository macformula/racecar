#include "can.hpp"

#include <linux/can.h>

#include <chrono>
#include <cstring>
#include <format>
#include <iostream>
#include <thread>

#include "shared/comms/can/msg.hpp"
#include "vcan/vcan.hpp"

static can_frame to_can_frame(const shared::can::RawMessage& msg) {
    struct can_frame frame {
        .can_id = msg.id, .can_dlc = msg.data_length,
    };
    std::memcpy(&frame.data, msg.data, msg.data_length);
    return frame;
}

namespace mcal::lnx::periph {

CanBase::CanBase(std::string iface) : socket_(iface) {}

void CanBase::Setup() {
    socket_.Open();

    // Another thread handles reading
    reader_thread_ = std::thread(&CanBase::StartReading, this);
}

void CanBase::Send(const shared::can::RawMessage& can_tx_msg) {
    std::cout << std::format("CanBase {}: Sending\n| {}", socket_.GetIface(),
                             can_tx_msg)
              << std::endl;

    auto frame = to_can_frame(can_tx_msg);
    socket_.Write(&frame);
}

void CanBase::StartReading() {
    struct can_frame frame;

    while (true) {
        // Block until a frame arrives
        ssize_t bytes_read = socket_.Read(&frame);
        if (bytes_read == -1) {
            perror("Error reading from CAN socket.");
            exit(1);
        }

        shared::can::RawMessage raw_msg(frame.can_id, true, frame.can_dlc,
                                        frame.data);
        std::cout << std::format("CanBase {}: Received\n| {}",
                                 socket_.GetIface(), raw_msg)
                  << std::endl;

        AddToBus(raw_msg);
    }
}

uint32_t CanBase::GetTimestamp() const {
    using namespace std::chrono;
    auto t = system_clock::now().time_since_epoch();
    auto ms = duration_cast<milliseconds>(t).count();
    return ms;
}

}  // namespace mcal::lnx::periph
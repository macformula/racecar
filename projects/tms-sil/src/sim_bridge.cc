/*
Encapsulate the network I/O, Protobuf state, and SIL mock peripheral objects
inside a dedicated bridge:

*/

#include "sim_bridge.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

bool TmsSilBridge::Start(const std::string& host, uint16_t port) {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);  // return -1 on error

    if (socket_fd_ < 0) {
        return false;
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if ((connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        close(socket_fd_);
        return false;
    }
    is_running_ = true;
    return true;
}

TmsSilBridge::Stop() {
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
    is_running_ = false;
}

TmsSilBridge::~TmsSilBridge() {
    Stop();
}

void TmsSilBridge::PollRx() {
    if (socket_fd_ < 0) {
        // connection is closed
        return;
    }

    uint8_t buf[512];
    ssize_t bytes_received =
        recv(socket_fd_, buf, sizeof(buf), MSG_DONTWAIT);  // non blocking read

    if (bytes_received <= 0) {
        return;  // no data receieved or connection error
    }

    macfe::tms::proto::SiltoTms rx_msg;

    if (!rx_msg.ParseFromArray(buf, static_cast<int>(bytes_received))) {
        std::cerr << "[SilBridge] Warning: Corrupt Protobuf packet received.\n";

        return;
    }
    int available_channels = rx_msg.adc_voltages_size();
    for (int i = 0; i < available_channels && i < 6; i++) {
        temp_adc[i].SetVoltage(rx_msg.adc_voltages(i));
    }
}

void TmsSilBridge::SendTx() {
    if (socket_fd_ < 0) return;

    macfe::tms::proto::TmstoSil tx_msg;

    tx_msg.set_fan_pwm_duty(fan_pwm.GetDutyCycle());

    std::string payload;
    if (!tx_msg.SerializeToString(&payload)) {
        return;  // serialization failed
    }

    send(socket_fd_, payload.data(), payload.size(), 0);
}

/*

namespace macfe::tms::proto {

class SimToTms : public google::protobuf::Message {
public:
    // Generated automatically from: repeated float adc_voltages = 1;
    int adc_voltages_size() const {
        return _internal_adc_voltages().size();
    }

    float adc_voltages(int index) const {
        return _internal_adc_voltages().Get(index);
    }

    bool ParseFromArray(const void* data, int size);
};

}  // namespace macfe::tms::proto



*/
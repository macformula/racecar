/// @author Samuel Parent
/// @date 2024-05-01

#pragma once

#include <grpcpp/grpcpp.h>

#include <memory>
#include <string>

#include "grpcpp/channel.h"
#include "signals.grpc.pb.h"
#include "signals.pb.h"

namespace mcal::raspi::sil {

class SilClient {
public:
    SilClient(std::string server_addr) {
        channel_ = grpc::CreateChannel(server_addr,
                                       grpc::InsecureChannelCredentials());
        stub_ = signals::Signals::NewStub(channel_);
    }

    bool ReadDigitalLevel(std::string ecu_name, std::string sig_name);
    double ReadAdcVoltage(std::string ecu_name, std::string sig_name);
    void SetDigitalLevel(std::string ecu_name, std::string sig_name,
                         bool level);

private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<signals::Signals::Stub> stub_;
};

}  // namespace mcal::raspi::sil

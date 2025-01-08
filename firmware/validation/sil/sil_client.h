/// @author Samuel Parent
/// @date 2024-05-01

#pragma once

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "grpcpp/channel.h"
#include "signals.grpc.pb.h"
#include "signals.pb.h"

namespace val::sil {

class SilClient {
public:
    SilClient(std::string server_addr);

    void Connect();

    void RegisterDigitalInput(std::string ecu_name, std::string sig_name);
    void RegisterDigitalOutput(std::string ecu_name, std::string sig_name);
    void RegisterAnalogInput(std::string ecu_name, std::string sig_name);

    bool ReadDigitalLevel(std::string ecu_name, std::string sig_name);
    double ReadAdcVoltage(std::string ecu_name, std::string sig_name);
    void SetDigitalLevel(std::string ecu_name, std::string sig_name,
                         bool level);

private:
    std::string addr_;
    std::unique_ptr<signals::Signals::Stub> stub_;

    void RegisterIO(std::string ecu_name, std::string sig_name, signals::SignalDirection sig_dir, 
        signals::SignalType sig_type);
    void SetRegister(std::string ecu_name, std::string sig_name, bool level);
    void ReadRegister(std::string ecu_name, std::string sig_name, signals::SignalDirection sig_dir, 
        signals::SignalType sig_type, signals::ReadSignalResponse &read_response);

};

}  // namespace val::sil

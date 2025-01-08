/// @author Samuel Parent
/// @date 2024-05-01

#include <google/protobuf/stubs/port.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/time.h>

#include <iostream>
#include <string>
#include <format>

#include "signals.grpc.pb.h"
#include "signals.pb.h"
#include "sil_client.h"

namespace val::sil {

SilClient::SilClient(std::string addr) : addr_(addr) {}

void SilClient::Connect() {
    stub_ = signals::Signals::NewStub(
        grpc::CreateChannel(addr_, grpc::InsecureChannelCredentials()));
}

void SilClient::RegisterDigitalInput(std::string ecu_name,
                                     std::string sig_name) {
    RegisterIO(ecu_name, sig_name, 
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT, signals::SignalType::SIGNAL_TYPE_DIGITAL);
}

void SilClient::RegisterDigitalOutput(std::string ecu_name,
                                      std::string sig_name) {
    RegisterIO(ecu_name, sig_name, 
        signals::SignalDirection::SIGNAL_DIRECTION_OUTPUT, signals::SignalType::SIGNAL_TYPE_DIGITAL);
}

void SilClient::RegisterAnalogInput(std::string ecu_name,
                                    std::string sig_name) {
    RegisterIO(ecu_name, sig_name, 
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT, signals::SignalType::SIGNAL_TYPE_ANALOG);

}

void SilClient::SetDigitalLevel(std::string ecu_name, std::string sig_name,
                                bool level) {
    SetRegister(ecu_name, sig_name, level);
}

bool SilClient::ReadDigitalLevel(std::string ecu_name, std::string sig_name) {
    signals::ReadSignalResponse read_response;

    ReadRegister(ecu_name, sig_name, 
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT, 
        signals::SignalType::SIGNAL_TYPE_ANALOG, read_response);

    return read_response.value_digital().level();

}

double SilClient::ReadAdcVoltage(std::string ecu_name, std::string sig_name) {
    signals::ReadSignalResponse read_response;

    ReadRegister(ecu_name, sig_name, 
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT, 
        signals::SignalType::SIGNAL_TYPE_ANALOG, read_response);

    if (!read_response.has_value_analog()) {
        std::cout << std::format("expected adc value got: {}\n", read_response.value_case());
    }

    return read_response.value_analog().voltage();
}

void SilClient::RegisterIO(std::string ecu_name, std::string sig_name, signals::SignalDirection sig_dir, signals::SignalType sig_type) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::RegisterSignalRequest register_request;
    signals::RegisterSignalResponse register_response;

    register_request.set_ecu_name(ecu_name);
    register_request.set_signal_name(sig_name);
    register_request.set_signal_direction(sig_dir);
    register_request.set_signal_type(sig_type);

    status =
        stub_->RegisterSignal(&context, register_request, &register_response);
    if (!status.ok()) {
        std::cout << std::format("{} : {}\n", status.error_code(), status.error_message());
    }

    if (!register_response.status()) {
        std::cout << std::format("register signal error: {}\n", register_response.error());
    }
}

void SilClient::SetRegister(std::string ecu_name, std::string sig_name, bool level) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::WriteSignalRequest write_request;
    signals::WriteSignalResponse write_response;

    write_request.set_ecu_name(ecu_name);
    write_request.set_signal_name(sig_name);
    write_request.mutable_value_digital()->set_level(level);

    status = stub_->WriteSignal(&context, write_request, &write_response);
    if (!status.ok()) {
        std::cout << std::format("{} : {}\n", status.error_code(), status.error_message());
        return;
    }

    if (!write_response.status()) {
        std::cout << std::format("write response error: {}\n", write_response.error());

        return;
    }
}

void SilClient::ReadRegister(std::string ecu_name, std::string sig_name, signals::SignalDirection sig_dir, 
    signals::SignalType sig_type, signals::ReadSignalResponse &read_response) {

    grpc::Status status;
    grpc::ClientContext context;
    signals::ReadSignalRequest read_request;

    read_request.set_ecu_name(ecu_name);
    read_request.set_signal_name(sig_name);
    read_request.set_signal_type(sig_type);
    read_request.set_signal_direction(sig_dir);

    status = stub_->ReadSignal(&context, read_request, read_response);
    if (!status.ok()) {
        std::cout << std::format("{} : {}\n", status.error_code(), status.error_message());

        return;
    }

    if (!read_response.status()) {
        std::cout << "write response error: " << read_response.error()
                  << std::endl;
        return;
    }

}

}  // namespace mcal::raspi::sil

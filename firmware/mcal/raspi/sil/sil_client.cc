/// @author Samuel Parent
/// @date 2024-05-01

#include <grpcpp/grpcpp.h>

#include <string>

#include "signals.grpc.pb.h"
#include "signals.pb.h"
#include "sil_client.h"

namespace mcal::raspi::sil {

void SilClient::RegisterDigitalInput(std::string ecu_name,
                                     std::string sig_name) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::RegisterSignalRequest register_request;
    signals::RegisterSignalResponse register_response;

    register_request.set_ecu_name(ecu_name);
    register_request.set_signal_name(sig_name);
    register_request.set_signal_direction(
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT);
    register_request.set_signal_type(signals::SignalType::SIGNAL_TYPE_DIGITAL);

    status =
        stub_->RegisterSignal(&context, register_request, &register_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
    }

    if (!register_response.status()) {
        std::cout << "register signal error: " << register_response.error()
                  << std::endl;
    }
}

void SilClient::RegisterDigitalOutput(std::string ecu_name,
                                      std::string sig_name) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::RegisterSignalRequest register_request;
    signals::RegisterSignalResponse register_response;

    register_request.set_ecu_name(ecu_name);
    register_request.set_signal_name(sig_name);
    register_request.set_signal_direction(
        signals::SignalDirection::SIGNAL_DIRECTION_OUTPUT);
    register_request.set_signal_type(signals::SignalType::SIGNAL_TYPE_DIGITAL);

    status =
        stub_->RegisterSignal(&context, register_request, &register_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
    }

    if (!register_response.status()) {
        std::cout << "register signal error: " << register_response.error()
                  << std::endl;
    }
}

void SilClient::RegisterAnalogInput(std::string ecu_name,
                                    std::string sig_name) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::RegisterSignalRequest register_request;
    signals::RegisterSignalResponse register_response;

    register_request.set_ecu_name(ecu_name);
    register_request.set_signal_name(sig_name);
    register_request.set_signal_direction(
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT);
    register_request.set_signal_type(signals::SignalType::SIGNAL_TYPE_ANALOG);

    status =
        stub_->RegisterSignal(&context, register_request, &register_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
    }

    if (!register_response.status()) {
        std::cout << "register signal error: " << register_response.error()
                  << std::endl;
    }
}

void SilClient::SetDigitalLevel(std::string ecu_name, std::string sig_name,
                                bool level) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::WriteSignalRequest write_request;
    signals::WriteSignalResponse write_response;

    write_request.set_ecu_name(ecu_name);
    write_request.set_signal_name(sig_name);
    write_request.mutable_value_digital()->set_level(level);

    status = stub_->WriteSignal(&context, write_request, &write_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return;
    }

    if (!write_response.status()) {
        std::cout << "write response error: " << write_response.error()
                  << std::endl;
        return;
    }
}

bool SilClient::ReadDigitalLevel(std::string ecu_name, std::string sig_name) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::ReadSignalRequest read_request;
    signals::ReadSignalResponse read_response;

    read_request.set_ecu_name(ecu_name);
    read_request.set_signal_name(sig_name);
    read_request.set_signal_type(signals::SignalType::SIGNAL_TYPE_DIGITAL);
    read_request.set_signal_direction(
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT);

    status = stub_->ReadSignal(&context, read_request, &read_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return false;
    }

    if (!read_response.status()) {
        std::cout << "write response error: " << read_response.error()
                  << std::endl;
        return false;
    }

    return read_response.value_digital().level();
}

double SilClient::ReadAdcVoltage(std::string ecu_name, std::string sig_name) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::ReadSignalRequest read_request;
    signals::ReadSignalResponse read_response;

    read_request.set_ecu_name(ecu_name);
    read_request.set_signal_name(sig_name);
    read_request.set_signal_type(signals::SignalType::SIGNAL_TYPE_ANALOG);
    read_request.set_signal_direction(
        signals::SignalDirection::SIGNAL_DIRECTION_INPUT);

    status = stub_->ReadSignal(&context, read_request, &read_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return 0.0;
    }

    if (!read_response.status()) {
        std::cout << "write response error: " << read_response.error()
                  << std::endl;
        return 0.0;
    }

    if (!read_response.has_value_analog()) {
        std::cout << "expected adc value got: " << read_response.value_case()
                  << std::endl;
    }

    return read_response.value_analog().voltage();
}

}  // namespace mcal::raspi::sil

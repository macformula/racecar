/// @author Samuel Parent
/// @date 2024-05-01

#include <grpcpp/grpcpp.h>

#include <string>

#include "signals.grpc.pb.h"
#include "signals.pb.h"
#include "sil_client.h"

namespace mcal::raspi::sil {

void SilClient::SetDigitalLevel(std::string ecu_name, std::string sig_name,
                                bool level) {
    grpc::Status status;
    grpc::ClientContext context;
    signals::DigitalSignal digital_signal;
    signals::WriteSignalRequest write_request;
    signals::WriteSignalResponse write_response;

    digital_signal.set_level(level);

    write_request.set_ecu_name(ecu_name);
    write_request.set_signal_name(sig_name);
    write_request.set_allocated_value_digital(&digital_signal);

    status = stub_->WriteSignal(&context, write_request, &write_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return;
    }

    if (!write_response.status()) {
        std::cout << "write response error" << write_response.error()
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

    status = stub_->ReadSignal(&context, read_request, &read_response);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return false;
    }

    if (!read_response.status()) {
        std::cout << "write response error" << read_response.error()
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

    if (!read_response.has_value_adc()) {
        std::cout << "expected adc value got: " << read_response.value_case()
                  << std::endl;
    }

    return read_response.value_adc().voltage();
}

}  // namespace mcal::raspi::sil

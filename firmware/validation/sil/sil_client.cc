/// @author Samuel Parent
/// @date 2024-05-01

#include "sil_client.h"

#include <format>
#include <iostream>
#include <string>

#include "signals_generated.h" 
#include "flatbuffers/flatbuffers.h"

using namespace signals;

namespace val::sil {

SilClient::SilClient(std::string addr) : addr_(addr) {}

void SilClient::Connect() {
    // socket driver
}

void SilClient::RegisterDigitalInput(std::string ecu_name,
                                     std::string sig_name) {
    flatbuffers::FlatBufferBuilder builder(1024);

    auto ecu_name = builder.CreateString(ecu_name);
    auto signal_name = builder.CreateString(sig_name);

    auto register_request = CreateRegisterRequest(builder, ecu_name, signal_name, SIGNAL_TYPE::DIGITAL, SIGNAL_DIRECTION::INPUT);
    auto request = CreateRequest(builder, RequestType::RegisterRequest, register_request.Union());
    builder.Finish(request);

    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();

    //send the buffer across tcp, check for response
}

void SilClient::RegisterDigitalOutput(std::string ecu_name, std::string sig_name) {
    flatbuffers::FlatBufferBuilder builder(1024);

    auto ecu_name = builder.CreateString(ecu_name);
    auto signal_name = builder.CreateString(sig_name);

    auto register_request = CreateRegisterRequest(builder, ecu_name, signal_name, SIGNAL_TYPE::DIGITAL, SIGNAL_DIRECTION::OUTPUT);
    auto request = CreateRequest(builder, RequestType::RegisterRequest, register_request.Union());
    builder.Finish(request);

    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();

    // send the buffer across tcp, check for response
}

void SilClient::RegisterAnalogInput(std::string ecu_name,
                                    std::string sig_name) {
    flatbuffers::FlatBufferBuilder builder(1024);

    auto ecu_name = builder.CreateString(ecu_name);
    auto signal_name = builder.CreateString(sig_name);

    auto register_request =
        CreateRegisterRequest(builder, ecu_name, signal_name,
                              SIGNAL_TYPE::ANALOG, SIGNAL_DIRECTION::OUTPUT);
    auto request = CreateRequest(builder, RequestType::RegisterRequest,
                                 register_request.Union());
    builder.Finish(request);

    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();
}

void SilClient::SetDigitalLevel(std::string ecu_name, std::string sig_name,
                                bool level) {
    flatbuffers::FlatBufferBuilder builder;

    // Create string fields
    auto ecu_name = builder.CreateString(ecu_name);
    auto signal_name = builder.CreateString(sig_name);

    // Create Digital signal value
    auto digital_value =
        CreateDigital(builder, level);  // Setting output pin to HIGH (true)

    // Create SignalValue union
    auto signal_value =
        CreateSignalValue(builder, SignalValue::Digital, digital_value.Union());

    // Create SetRequest object
    auto set_request =
        CreateSetRequest(builder, ecu_name, signal_name, SIGNAL_TYPE::DIGITAL, signal_value, SIGNAL_DIRECTION::OUTPUT);

    // Create Request union
    auto request =
        CreateRequest(builder, RequestType::SetRequest, set_request.Union());

    builder.Finish(request);

    // Get the serialized buffer pointer
    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();

    // send over tcp
    }
}

bool SilClient::ReadDigitalLevel(std::string ecu_name, std::string sig_name) {
    flatbuffers::FlatBufferBuilder builder;

    // Create string fields
    auto ecu_name = builder.CreateString(ecu_name);
    auto signal_name = builder.CreateString(sig_name);

    // Create ReadRequest object
    auto read_request = CreateReadRequest(builder, ecu_name, signal_name, SIGNAL_TYPE::DIGITAL, SIGNAL_DIRECTION::OUTPUT);

    // Create Request union
    auto request = CreateRequest(builder, RequestType::ReadRequest, read_request.Union());

    builder.Finish(request);

    // Get the serialized buffer pointer
    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();

    // set over tcp and return bool response
}

double SilClient::ReadAdcVoltage(std::string ecu_name, std::string sig_name) {
    flatbuffers::FlatBufferBuilder builder;

    // Create string fields
    auto ecu_name = builder.CreateString(ecu_name);
    auto signal_name = builder.CreateString(sig_name);

    // Create ReadRequest object
    auto read_request =
        CreateReadRequest(builder, ecu_name, signal_name, SIGNAL_TYPE::ANALOG,
                          SIGNAL_DIRECTION::OUTPUT);

    // Create Request union
    auto request =
        CreateRequest(builder, RequestType::ReadRequest, read_request.Union());

    builder.Finish(request);

    // Get the serialized buffer pointer
    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();
}

// namespace val::sil

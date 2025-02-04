/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

#include <cstdint>

#include "shared/device/dac.hpp"
#include "shared/util/peripheral.hpp"

namespace shared::periph {

class AnalogOutput : public util::Peripheral {
public:
    virtual void SetVoltage(float voltage) = 0;
};

class AnalogOutputExternalDac : public AnalogOutput {
public:
    AnalogOutputExternalDac(shared::device::DigitalAnalogConverter& dac,
                            uint8_t channel)
        : dac_(dac), channel_(channel) {}

    void SetVoltage(float voltage) override {
        dac_.SetVoltage(channel_, voltage);
    }

    float GetVoltage() const override {
        return dac_.GetVoltageOutput(channel_);
    }

private:
    shared::device::DigitalAnalogConverter& dac_;
    uint8_t channel_;
};

}  // namespace shared::periph
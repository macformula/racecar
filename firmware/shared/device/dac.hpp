/// @author Samuel Parent
/// @date 2025-02-04

#pragma once

#include <cstdint>

#include "shared/util/device.hpp"

namespace shared::device {

class DigitalAnalogConverter : public util::Device {
public:
    virtual void SetVoltage(uint8_t channel, float value) = 0;
    virtual float GetVoltageOutput(uint8_t channel) = 0;
};

}  // namespace shared::device

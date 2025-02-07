/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

#include <_types/_uint8_t.h>

#include <cstdint>

#include "shared/util/peripheral.hpp"

namespace shared::periph {

class AnalogOutput : public util::Peripheral {
public:
    virtual void SetVoltage(float voltage) = 0;
    virtual float GetLoadedVoltage() = 0;
    virtual float GetVoltageSetpoint() = 0;
    virtual void LoadVoltage() = 0;
    virtual void SetAndLoadVoltage(float voltage) = 0;
};

/**
 * An AnalogOutputGroup represents multiple analog outputs that should be loaded
 * simultaneously. This is useful for applications where multiple outputs need
 * to change at the same time, such as coordinated motion control or
 * synchronized waveform generation.
 */
template <uint8_t num_channels>
    requires(num_channels > 0)
class AnalogOutputGroup : public util::Peripheral {
public:
    virtual void SetVoltage(uint8_t channel, float voltage) = 0;
    virtual float GetLoadedVoltage(uint8_t channel) = 0;
    virtual float GetVoltageSetpoint(uint8_t channel) = 0;
    virtual void LoadVoltages() = 0;
    virtual void SetAndLoadAllVoltages(float voltages[num_channels]) = 0;
};

}  // namespace shared::periph
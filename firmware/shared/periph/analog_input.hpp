/// @author Samuel Shi
/// @date 2024-11-17
/// Modified from adc.h

#pragma once

#include "shared/util/peripheral.hpp"

namespace shared::periph {

class AnalogInput : public util::Peripheral {
public:
    /// @brief Turn on the peripheral
    virtual void Start() = 0;

    /// @brief Read the voltage
    virtual float Read() = 0;
};

}  // namespace shared::periph
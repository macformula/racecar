/// @author Samuel Shi
/// @date 2024-11-17
/// Modified from adc.h

#pragma once

namespace macfe::periph {

class AnalogInput {
public:
    virtual float ReadVoltage() = 0;
};

}  // namespace macfe::periph
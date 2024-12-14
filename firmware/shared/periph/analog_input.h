/// @author Samuel Shi 
/// @date 2024-11-17
/// Modified from adc.h 

#pragma once

#include "shared/util/peripheral.h"

namespace shared::periph {

class AnalogInput : public util::Peripheral {
public:
    virtual void Start() = 0;
    virtual float Read() = 0;
};

}  // namespace shared::periph
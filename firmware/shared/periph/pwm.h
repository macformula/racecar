/// @author Blake Freer
/// @date 2023-11-09

#pragma once

#include "shared/util/peripheral.h"

namespace shared::periph {

class PWMOutput : public util::Peripheral {
public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void SetDutyCycle(float duty_cycle) = 0;
    virtual float GetDutyCycle() = 0;
};

}  // namespace shared::periph

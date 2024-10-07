/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

#include "shared/util/peripheral.h"

namespace shared::periph {

class AnalogOutput : public util::Peripheral {
public:
    virtual void Set(float voltage) = 0;
};

}  // namespace shared::periph
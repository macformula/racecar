/// @author Blake Freer
/// @date 2023-11-09

#pragma once

#include <cstdint>

#include "shared/util/peripheral.hpp"

namespace shared::periph {

class ADCInput : public util::Peripheral {
public:
    virtual void Start() = 0;
    virtual uint32_t Read() = 0;
};

}  // namespace shared::periph
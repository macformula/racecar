/// @author Blake Freer
/// @date 2023-11-08

#ifndef SHARED_PERIPH_GPIO_H_
#define SHARED_PERIPH_GPIO_H_

#include "shared/util/peripheral.h"

namespace shared::periph {

class DigitalInput : public util::Peripheral {
public:
    virtual bool Read() = 0;
};

class DigitalOutput : public util::Peripheral {
public:
    virtual void SetHigh() = 0;
    virtual void SetLow() = 0;
    virtual void Set(bool value) = 0;
};

}  // namespace shared::periph

#endif
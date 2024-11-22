#pragma once

#include "shared/periph/gpio.h"

namespace bindings {

    extern shared::periph::DigitalOutput& indicator;
    extern void DelayMS(unsigned int ms);
    extern void Initialize();

    }  // namespace bindings

    extern void ToggleInterruptHandler();

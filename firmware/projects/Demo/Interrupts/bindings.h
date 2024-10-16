#pragma once

#include "shared/periph/gpio.h"

namespace bindings {

    extern shared::periph::DigitalOutput& indicator;
    extern void DelayMS(unsigned int ms);
    extern void Initialize();
    extern void AppLevelFunction();
    void setup_interupt();

    
    
}  // namespace bindings
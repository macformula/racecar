#pragma once

#include "shared/periph/gpio.h"

namespace bindings {

    extern shared::periph::DigitalOutput& indicator;
    extern void DelayMS(unsigned int ms);
    extern void Initialize();
    void setup_interupt();
    
    extern bool toggle;
    
}  // namespace bindings
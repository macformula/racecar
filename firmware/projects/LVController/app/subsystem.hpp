/// @author Blake Freer
/// @date 2024-03-06

#pragma once

#include "shared/periph/gpio.hpp"

/// @brief A Subsystem which can be enabled / disabled.
class Subsystem {
public:
    // Use shared::periph::InverseDigitalOutput if the subsystem is enabled by
    // a low signal.
    Subsystem(shared::periph::DigitalOutput& enable_output);

    virtual void Enable();
    virtual void Disable();

private:
    shared::periph::DigitalOutput& enable_output_;
};

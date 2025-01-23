#include "subsystem.hpp"

#include "shared/periph/gpio.hpp"

Subsystem::Subsystem(shared::periph::DigitalOutput& enable_output)
    : enable_output_(enable_output) {}

void Subsystem::Enable() {
    enable_output_.SetHigh();
}
inline void Subsystem::Disable() {
    enable_output_.SetLow();
}

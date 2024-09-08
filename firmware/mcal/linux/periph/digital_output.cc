#include <iostream>
#include <string>

#include "gpio.h"

namespace mcal::linux::periph {

DigitalOutput::DigitalOutput(std::string name) : name_(name) {}

void DigitalOutput::Set(bool value) {
    std::cout << "Setting DigitalOutput \"" << name_ << "\" to "
              << (value ? "true" : "false") << std::endl;
}

void DigitalOutput::SetHigh() {
    Set(true);
}

void DigitalOutput::SetLow() {
    Set(false);
}

}  // namespace mcal::linux::periph
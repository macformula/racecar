#include "digital_output.hpp"

#include <format>
#include <iostream>
#include <string>

namespace mcal::lnx::periph {

DigitalOutput::DigitalOutput(std::string name) : name_(name) {}

void DigitalOutput::Set(bool value) {
    std::cout << std::format("DigitalOutput \"{}\" => {}", name_,
                             value ? "true" : "false")
              << std::endl;
}

void DigitalOutput::SetHigh() {
    Set(true);
}

void DigitalOutput::SetLow() {
    Set(false);
}

}  // namespace mcal::lnx::periph
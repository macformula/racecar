#include "digital_input.hpp"

#include <fmt/core.h>
#include <iostream>
#include <string>

namespace mcal::lnx {

DigitalInput::DigitalInput(std::string name) : name_(name) {};

bool DigitalInput::Read() {
    int value;

    std::cout << fmt::format("Reading DigitalInput \"{}\"", name_) << std::endl;
    std::cout << "| Enter 0 for False, 1 for True: ";
    std::cin >> value;
    std::cout << fmt::format("| Value was {}", value ? "true" : "false")
              << std::endl;

    return value;
}

}  // namespace mcal::lnx
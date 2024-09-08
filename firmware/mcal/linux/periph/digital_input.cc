#include <iostream>
#include <string>

#include "digital_input.h"

namespace mcal::linux::periph {

DigitalInput::DigitalInput(std::string name) : name_(name) {};

bool DigitalInput::Read() {
    int value;

    std::cout << "Reading DigitalInput \"" << name_ << "\"" << std::endl;
    std::cout << "| Enter 0 for False, 1 for True: ";
    std::cin >> value;
    std::cout << "| Value was " << (value ? "true" : "false") << std::endl;

    return value;
}

}  // namespace mcal::linux::periph
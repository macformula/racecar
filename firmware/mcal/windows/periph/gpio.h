/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_GPIO_H_
#define MCAL_WINDOWS_PERIPH_GPIO_H_

#include <iostream>
#include <string>

#include "shared/util/peripheral.h"

namespace mcal::periph {

class DigitalInput : public shared::util::Peripheral {
private:
    std::string name_;

public:
    DigitalInput(std::string name) : name_(name) {}

    bool Read() {
        int value;
        std::cout << "Reading DigitalInput " << name_ << std::endl;
        std::cout << " | Enter 0 for False, 1 for True: ";
        std::cin >> value;
        std::cout << " | Value was " << (value ? "true" : "false") << std::endl;
        return value;
    }
};

class DigitalOutput : public shared::util::Peripheral {
private:
    std::string name_;

public:
    DigitalOutput(std::string name) : name_(name) {}

    void Set(bool value) {
        std::cout << "Setting DigitalOutput " << name_ << " to "
                  << (value ? "true" : "false") << std::endl;
    }

    void SetHigh() {
        Set(true);
    }

    void SetLow() {
        Set(false);
    }
};

}  // namespace mcal::periph

#endif
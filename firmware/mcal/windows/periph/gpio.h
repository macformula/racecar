/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_GPIO_H_
#define MCAL_WINDOWS_PERIPH_GPIO_H_

#include <iostream>

#include "shared/periph/gpio.h"

namespace mcal::periph {

class DigitalInput : public shared::periph::DigitalInput {
private:
    int channel_;

public:
    DigitalInput(int channel) : channel_(channel) {}

    bool Read() override {
        int value;
        std::cout << "Reading DigitalInput Channel " << channel_ << std::endl;
        std::cout << " | Enter 0 for False, 1 for True: ";
        std::cin >> value;
        std::cout << " | Value was " << (value ? "true" : "false") << std::endl;
        return value;
    }
};

class DigitalOutput : public shared::periph::DigitalOutput {
private:
    int channel_;

public:
    DigitalOutput(int channel) : channel_(channel) {}

    void Set(bool value) override {
        std::cout << "Setting DigitalOutput Channel " << channel_ << " to "
                  << (value ? "true" : "false") << std::endl;
    }

    void SetHigh() override {
        Set(true);
    }

    void SetLow() override {
        Set(false);
    }
};

}  // namespace mcal::periph

#endif
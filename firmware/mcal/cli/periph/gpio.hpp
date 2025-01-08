/// @author Blake Freer
/// @date 2023-12-10

#pragma once

#include <format>
#include <iostream>
#include <string>

#include "shared/periph/gpio.hpp"

namespace mcal::cli::periph {

class DigitalInput : public shared::periph::DigitalInput {
private:
    std::string name_;

public:
    DigitalInput(std::string name) : name_(name) {}

    bool Read() override {
        int value;
        std::cout << std::format("Reading DigitalInput {}", name_) << std::endl;
        std::cout << " | Enter 0 for False, 1 for True: ";
        std::cin >> value;
        std::cout << std::format(" | Value was {}", value) << std::endl;
        return value;
    }
};

class DigitalOutput : public shared::periph::DigitalOutput {
private:
    std::string name_;

public:
    DigitalOutput(std::string name) : name_(name) {}

    void Set(bool value) override {
        std::cout << std::format("Setting DigitalOutput Channel {} to {}",
                                 name_, value)
                  << std::endl;
    }

    void SetHigh() override {
        Set(true);
    }

    void SetLow() override {
        Set(false);
    }
};

}  // namespace mcal::cli::periph

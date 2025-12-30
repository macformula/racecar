/// @author Blake Freer
/// @date 2023-12-10

#pragma once

#include <fmt/core.h>
#include <iostream>
#include <string>

#include "periph/gpio.hpp"

namespace mcal::cli {

class DigitalInput : public macfe::periph::DigitalInput {
private:
    std::string name_;

public:
    DigitalInput(std::string name) : name_(name) {}

    bool Read() override {
        int value;
        std::cout << fmt::format("Reading DigitalInput {}", name_) << std::endl;
        std::cout << " | Enter 0 for False, 1 for True: ";
        std::cin >> value;
        std::cout << fmt::format(" | Value was {}", value) << std::endl;
        return value;
    }
};

class DigitalOutput : public macfe::periph::DigitalOutput {
private:
    std::string name_;

public:
    DigitalOutput(std::string name) : name_(name) {}

    void Set(bool value) override {
        std::cout << fmt::format("Setting DigitalOutput Channel {} to {}",
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

}  // namespace mcal::cli

#pragma once

#include <fmt/core.h>

#include <iostream>
#include <string>

#include "periph/gpio.hpp"

namespace mcal::sil {

class DigitalInput : public macfe::periph::DigitalInput {
private:
    bool* input_;

public:
    DigitalInput(bool* input) : input_(input) {}

    bool Read() override {
        return *input_;
    }
};

class DigitalOutput : public macfe::periph::DigitalOutput {
private:
    bool* output_;

public:
    DigitalOutput(bool* output) : output_(output) {}

    void Set(bool level) override {
        std::cout << fmt::format("setting {}.{} {}", ecu_name_, sig_name_,
                                 level)
                  << std::endl;
        return sil_client_.SetDigitalLevel(ecu_name_, sig_name_, level);
    }

    void SetHigh() override {
        Set(true);
    }
    void SetLow() override {
        Set(false);
    }
};
}  // namespace mcal::sil
#pragma once

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
    void Set(bool value) override {
        *output_ = value;
    }
    void SetHigh() override {
        Set(true);
    }
    void SetLow() override {
        Set(false);
    }
};
}  // namespace mcal::sil
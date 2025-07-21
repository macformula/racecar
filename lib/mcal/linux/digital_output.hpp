#pragma once

#include <iostream>
#include <string>

#include "periph/gpio.hpp"

namespace mcal::lnx {
class DigitalOutput : public macfe::periph::DigitalOutput {
public:
    DigitalOutput(std::string name);

    void Set(bool value) override;
    void SetHigh() override;
    void SetLow() override;

private:
    std::string name_;
};

}  // namespace mcal::lnx
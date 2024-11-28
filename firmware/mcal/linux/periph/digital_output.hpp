#pragma once

#include <iostream>
#include <string>

#include "shared/periph/gpio.hpp"

namespace mcal::lnx::periph {
class DigitalOutput : public shared::periph::DigitalOutput {
public:
    DigitalOutput(std::string name);

    void Set(bool value) override;
    void SetHigh() override;
    void SetLow() override;

private:
    std::string name_;
};

}  // namespace mcal::lnx::periph
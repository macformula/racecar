#pragma once

#include <string>

#include "periph/gpio.hpp"

namespace mcal::lnx {

class DigitalInput : public macfe::periph::DigitalInput {
public:
    DigitalInput(std::string name);

    bool Read() override;

private:
    std::string name_;
};

}  // namespace mcal::lnx
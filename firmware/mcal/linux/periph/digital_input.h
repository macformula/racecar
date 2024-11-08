#pragma once

#include <string>

#include "shared/periph/gpio.h"

namespace mcal::lnx::periph {

class DigitalInput : public shared::periph::DigitalInput {
public:
    DigitalInput(std::string name);

    bool Read() override;

private:
    std::string name_;
};

}  // namespace mcal::lnx::periph
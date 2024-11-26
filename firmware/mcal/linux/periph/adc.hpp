#pragma once

#include <cstdint>
#include <string>

#include "shared/periph/adc.hpp"

namespace mcal::lnx::periph {

class ADCInput : public shared::periph::ADCInput {
public:
    ADCInput(std::string name);

    void Start() override;
    uint32_t Read() override;

private:
    std::string name_;
};

}  // namespace mcal::lnx::periph
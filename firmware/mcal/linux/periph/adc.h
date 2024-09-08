#pragma once

#include <cstdint>
#include <string>

#include "shared/periph/adc.h"

namespace mcal::linux::periph {

class ADCInput : public shared::periph::ADCInput {
public:
    ADCInput(std::string name);

    void Start() override;
    uint32_t Read() override;

private:
    std::string name_;
};

}  // namespace mcal::linux::periph
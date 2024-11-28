/// @author Blake Freer
/// @date 2023-12-10

#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#include "shared/periph/adc.hpp"

namespace mcal::cli::periph {

class ADCInput : public shared::periph::ADCInput {
public:
    ADCInput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << "Reading ADC " << name_ << "..." << std::endl;
    }

    uint32_t Read() override {
        Start();
        uint32_t adc_val;
        std::cout << " | Enter an unsigned 32-bit value: ";
        std::cin >> adc_val;
        std::cout << " | Obtained value " << adc_val << std::endl;
        return adc_val;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli::periph

/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_ADC_H_
#define MCAL_WINDOWS_PERIPH_ADC_H_

#include <cstdint>
#include <iostream>

#include "shared/periph/adc.h"

namespace mcal::periph {

class ADCInput : public shared::periph::ADCInput {
public:
    ADCInput(int channel) : channel_(channel) {}

    void Start() override {
        std::cout << "Reading ADC Channel " << channel_ << "..." << std::endl;
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
    int channel_;
};

}  // namespace mcal::periph

#endif
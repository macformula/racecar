/// @author Samuel parent
/// @date 2024-05-01

#pragma once

#include <cstdint>
#include <string>

#include "client.h"
#include "periph/adc.hpp"

namespace mcal::raspi {

class ADCInput : public macfe::periph::ADCInput {
public:
    ADCInput(std::string ecu_name, std::string sig_name,
             val::sil::SilClient& sil_client)
        : ecu_name_(ecu_name), sig_name_(sig_name), sil_client_(sil_client) {}

    void Register() {
        sil_client_.RegisterAnalogInput(ecu_name_, sig_name_);
    }

    void Start() override {}

    uint32_t Read() override {
        double adc_voltage = sil_client_.ReadAdcVoltage(ecu_name_, sig_name_);

        return voltage_to_adc.Evaluate(adc_voltage);
    }

private:
    static constexpr double kMaxVoltage = 3.3;
    static constexpr uint8_t kNumAdcBits = 12;
    static constexpr uint32_t kMaxAdcValue = (1 << kNumAdcBits) - 1;
    static constexpr double kVoltsToAdc = (kMaxAdcValue / kMaxVoltage);

    macfe::util::LinearMap<double, uint32_t> voltage_to_adc{kVoltsToAdc, 0};

    std::string ecu_name_;
    std::string sig_name_;
    val::sil::SilClient& sil_client_;
};

}  // namespace mcal::raspi

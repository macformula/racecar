/// @author Samuel Parent
/// @date 2025-02-04

#pragma once

#include <cstdint>

#include "shared/periph/analog_output.hpp"
#include "shared/util/device.hpp"

namespace shared::device {

template <uint8_t num_channels>
    requires(num_channels > 0)
class DigitalAnalogConverter : public util::Device,
                               public periph::AnalogOutputGroup<num_channels> {
public:
    virtual void SetVoltage(uint8_t channel, float voltage) = 0;
    virtual float GetLoadedVoltage(uint8_t channel) = 0;
    virtual float GetVoltageSetpoint(uint8_t channel) = 0;
    virtual void LoadVoltages() = 0;
    virtual void SetAndLoadAllVoltages(float voltages[num_channels]) = 0;
};

template <uint8_t num_external_dacs, uint8_t num_channels_per_dac>
    requires(num_external_dacs > 0)
class DigitalAnalogConverterGroup
    : public periph::AnalogOutputGroup<num_external_dacs *
                                       num_channels_per_dac> {
public:
    static constexpr uint8_t kTotalChannels =
        num_external_dacs * num_channels_per_dac;

    DigitalAnalogConverterGroup(DigitalAnalogConverter<num_channels_per_dac>* (
        &dacs)[num_external_dacs])
        : dacs_(dacs) {}
    virtual ~DigitalAnalogConverterGroup() = default;

    void SetVoltage(uint8_t channel, float voltage) override {
        uint8_t dac_index = channel / num_channels_per_dac;
        uint8_t dac_channel = channel % num_channels_per_dac;

        if (dac_index < num_external_dacs) {
            dacs_[dac_index]->SetVoltage(dac_channel, voltage);
        }
    }

    void LoadVoltages() override {
        for (uint8_t i = 0; i < num_external_dacs; i++) {
            dacs_[i]->LoadVoltages();
        }
    }

    void SetAndLoadAllVoltages(float voltages[kTotalChannels]) override {
        for (uint8_t i = 0; i < kTotalChannels; i++) {
            SetVoltage(i, voltages[i]);
        }
        LoadVoltages();
    }

    float GetLoadedVoltage(uint8_t channel) override {
        uint8_t dac_index = channel / num_channels_per_dac;
        uint8_t dac_channel = channel % num_channels_per_dac;

        return dacs_[dac_index]->GetLoadedVoltage(dac_channel);
    }

    float GetVoltageSetpoint(uint8_t channel) override {
        uint8_t dac_index = channel / num_channels_per_dac;
        uint8_t dac_channel = channel % num_channels_per_dac;

        return dacs_[dac_index]->GetVoltageSetpoint(dac_channel);
    }

private:
    DigitalAnalogConverter<num_channels_per_dac>* (&dacs_)[num_external_dacs];
};

// Deduction guide
template <uint8_t num_external_dacs, uint8_t num_channels_per_dac>
DigitalAnalogConverterGroup(
    DigitalAnalogConverter<num_channels_per_dac>* (&dacs)[num_external_dacs])
    -> DigitalAnalogConverterGroup<num_external_dacs, num_channels_per_dac>;

}  // namespace shared::device

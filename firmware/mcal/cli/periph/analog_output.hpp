/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

#include <_types/_uint8_t.h>

#include <iomanip>
#include <iostream>
#include <string>

#include "shared/periph/analog_output.hpp"

namespace mcal::cli::periph {

class AnalogOutput : public shared::periph::AnalogOutput {
public:
    AnalogOutput(std::string name)
        : name_(name), voltage_setpoint_(0.0F), voltage_loaded_(0.0F) {}

    void SetVoltage(float voltage) override {
        std::cout << "Setting AnalogOutput " << name_ << " to " << std::fixed
                  << std::setprecision(3) << voltage << " V" << std::endl;
        voltage_setpoint_ = voltage;
    }

    float GetVoltageSetpoint() override {
        return voltage_setpoint_;
    }

    float GetLoadedVoltage() override {
        return voltage_loaded_;
    }

    void LoadVoltage() override {
        std::cout << "Loading AnalogOutput " << name_ << " to " << std::fixed
                  << std::setprecision(3) << voltage_setpoint_ << " V"
                  << std::endl;
        voltage_loaded_ = voltage_setpoint_;
    }

    void SetAndLoadVoltage(float voltage) override {
        SetVoltage(voltage);
        LoadVoltage();
    }

private:
    std::string name_;
    float voltage_setpoint_;
    float voltage_loaded_;
};

template <uint8_t num_channels>
class AnalogOutputGroup
    : public shared::periph::AnalogOutputGroup<num_channels> {
public:
    AnalogOutputGroup(std::string name, AnalogOutput (&outputs)[num_channels])
        : name_(name), outputs_(outputs) {}

    void SetVoltage(uint8_t channel, float voltage) override {
        std::cout << "Setting AnalogOutputGroup " << name_ << " channel "
                  << static_cast<int>(channel) << ":" << std::endl;
        if (channel < num_channels) {
            outputs_[channel].SetVoltage(voltage);
        }
    }

    float GetVoltageSetpoint(uint8_t channel) override {
        if (channel < num_channels) {
            return outputs_[channel].GetVoltageSetpoint();
        }
        return 0.0F;
    }

    float GetLoadedVoltage(uint8_t channel) override {
        if (channel < num_channels) {
            return outputs_[channel].GetLoadedVoltage();
        }
        return 0.0F;
    }

    void LoadVoltages() override {
        std::cout << "Loading AnalogOutputGroup " << name_
                  << " channels:" << std::endl;
        for (uint8_t i = 0; i < num_channels; i++) {
            if (i < num_channels) {
                outputs_[i].LoadVoltage();
            }
        }
    }

    void SetAndLoadAllVoltages(float voltages[num_channels]) override {
        std::cout << "Setting and loading all channels for " << name_ << ":"
                  << std::endl;
        for (uint8_t i = 0; i < num_channels; i++) {
            if (i < num_channels) {
                outputs_[i].SetVoltage(voltages[i]);
            }
        }
        LoadVoltages();
    }

private:
    std::string name_;
    AnalogOutput (&outputs_)[num_channels];
};

}  // namespace mcal::cli::periph

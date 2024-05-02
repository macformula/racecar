/// @author Samuel Parent
/// @date 2023-12-10

#pragma once

#include <string>

#include "mcal/raspi/sil/sil_client.h"
#include "shared/periph/gpio.h"

namespace mcal::raspi::periph {

class DigitalInput : public shared::periph::DigitalInput {
public:
    DigitalInput(std::string ecu_name, std::string sig_name,
                 mcal::raspi::sil::SilClient& sil_client)
        : ecu_name_(ecu_name), sig_name_(sig_name), sil_client_(sil_client) {}

    bool Read() override {
        return sil_client_.ReadDigitalLevel(ecu_name_, sig_name_);
    }

private:
    std::string ecu_name_;
    std::string sig_name_;
    mcal::raspi::sil::SilClient& sil_client_;
};

class DigitalOutput : public shared::periph::DigitalOutput {
public:
    DigitalOutput(std::string ecu_name, std::string sig_name,
                 mcal::raspi::sil::SilClient& sil_client)
        : ecu_name_(ecu_name), sig_name_(sig_name), sil_client_(sil_client) {}

    void Set(bool level) override {
        return sil_client_.SetDigitalLevel(ecu_name_, sig_name_, level);
    }

    void SetHigh() override {
        return sil_client_.SetDigitalLevel(ecu_name_, sig_name_, true);
    }

    void SetLow() override {
        return sil_client_.SetDigitalLevel(ecu_name_, sig_name_, true);
    }

private:
    std::string ecu_name_;
    std::string sig_name_;
    mcal::raspi::sil::SilClient& sil_client_;
};

}  // namespace mcal::raspi::periph

/// @author Samuel Parent
/// @date 2023-12-10

#pragma once

#include <format>
#include <iostream>
#include <string>

#include "shared/periph/gpio.hpp"
#include "validation/sil/sil_client.h"

namespace mcal::raspi {

class DigitalInput : public shared::periph::DigitalInput {
public:
    DigitalInput(std::string ecu_name, std::string sig_name,
                 val::sil::SilClient& sil_client)
        : ecu_name_(ecu_name), sig_name_(sig_name), sil_client_(sil_client) {}

    void Register() {
        sil_client_.RegisterDigitalInput(ecu_name_, sig_name_);
    }

    bool Read() override {
        return sil_client_.ReadDigitalLevel(ecu_name_, sig_name_);
    }

private:
    std::string ecu_name_;
    std::string sig_name_;
    val::sil::SilClient& sil_client_;
};

class DigitalOutput : public shared::periph::DigitalOutput {
public:
    DigitalOutput(std::string ecu_name, std::string sig_name,
                  val::sil::SilClient& sil_client)
        : ecu_name_(ecu_name), sig_name_(sig_name), sil_client_(sil_client) {}

    void Register() {
        sil_client_.RegisterDigitalOutput(ecu_name_, sig_name_);
    }

    void Set(bool level) override {
        std::cout << std::format("setting {}.{} {}", ecu_name_, sig_name_,
                                 level)
                  << std::endl;
        return sil_client_.SetDigitalLevel(ecu_name_, sig_name_, level);
    }

    void SetHigh() override {
        Set(true);
    }

    void SetLow() override {
        return sil_client_.SetDigitalLevel(ecu_name_, sig_name_, true);
    }

private:
    std::string ecu_name_;
    std::string sig_name_;
    val::sil::SilClient& sil_client_;
};

}  // namespace mcal::raspi

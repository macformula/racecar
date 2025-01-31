#pragma once

#include "shared/periph/gpio.hpp"

/// @brief Tractive System Active Indicator
/// A light on the car that indicates if the car can drive or is faulted.
class TSSI {
public:
    TSSI(shared::periph::DigitalOutput& enable,
         shared::periph::DigitalOutput& green,
         shared::periph::DigitalOutput& red);

    void Enable();
    void Disable();
    void Update(bool bms_fault, bool imd_fault, int time_ms);

private:
    shared::periph::DigitalOutput& enable_;
    shared::periph::DigitalOutput& green_;
    shared::periph::DigitalOutput& red_;

    bool flash_toggle_ = true;
    int state_entered_time_ = 0;
};
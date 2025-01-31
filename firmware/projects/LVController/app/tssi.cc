#include "tssi.hpp"

TSSI::TSSI(shared::periph::DigitalOutput& enable,
           shared::periph::DigitalOutput& green,
           shared::periph::DigitalOutput& red)
    : enable_(enable), green_(green), red_(red) {}

void TSSI::Enable() {
    enable_.SetHigh();
}
void TSSI::Disable() {
    enable_.SetLow();
}

/// @brief See FSAE 2025 Rules EV.5.11.5
void TSSI::Update(bool bms_fault, bool imd_fault, int time_ms) {
    const float kFlashFrequencyHz = 4.0f;  // must be between 2Hz - 5 Hz
    // /2 since one period is two toggles
    const int kTogglePeriodMs = 1000.f / kFlashFrequencyHz / 2.f;

    bool fault = bms_fault || imd_fault;

    if (!fault) {
        red_.SetLow();
        green_.SetHigh();
        state_entered_time_ = time_ms;
        flash_toggle_ = true;
    } else {
        green_.SetLow();
        red_.Set(flash_toggle_);
        if (time_ms - state_entered_time_ > kTogglePeriodMs) {
            state_entered_time_ = time_ms;
            flash_toggle_ = !flash_toggle_;
        }
    }
}

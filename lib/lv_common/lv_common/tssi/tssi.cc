#include "tssi.hpp"

#include "bindings.hpp"

namespace tssi {

static const float kFlashFrequencyHz = 4.0f;  // must be between 2Hz - 5 Hz
// /2 since one period is two toggles
static const int kTogglePeriodMs = (1000.f / kFlashFrequencyHz) / 2.f;

static bool flash_toggle = true;
static uint32_t state_entered_time = 0;

static bool enabled = false;
static bool imd_fault;
static bool bms_fault;

void SetEnabled(bool enable) {
    enabled = enable;
}

bool GetImdFault(void) {
    return imd_fault;
}

bool GetBmsFault(void) {
    return bms_fault;
}

/// @brief See FSAE 2025 Rules EV.5.11.5
void task_10hz(void) {
    bindings::tssi_en.Set(enabled);

    imd_fault = bindings::imd_fault.Read();
    bms_fault = bindings::bms_fault.Read();

    uint32_t time_ms = bindings::GetTick();

    if (bms_fault || imd_fault) {
        bindings::tssi_green_signal.SetLow();
        bindings::tssi_red_signal.Set(flash_toggle);
        if ((time_ms - state_entered_time) > kTogglePeriodMs) {
            state_entered_time = time_ms;
            flash_toggle = !flash_toggle;
        }
    } else {
        bindings::tssi_red_signal.SetLow();
        bindings::tssi_green_signal.SetHigh();
        flash_toggle = true;
    }
}

}  // namespace tssi

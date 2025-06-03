#include "tssi.hpp"

#include "bindings.hpp"

namespace tssi {

static const float kFlashFrequencyHz = 4.0f;  // must be between 2Hz - 5 Hz
// /2 since one period is two toggles
static const int kTogglePeriodMs = (1000.f / kFlashFrequencyHz) / 2.f;

static bool flash_toggle = true;
static uint32_t state_entered_time = 0;

static bool enabled = false;

void SetEnabled(bool enable) {
    enabled = enable;
}

/// @brief See FSAE 2025 Rules EV.5.11.5
void task_10hz(generated::can::VehBus& veh_can) {
    bindings::tssi_en.Set(enabled);

    bool imd_fault = bindings::imd_fault.Read();
    bool bms_fault = bindings::bms_fault.Read();

    veh_can.Send(generated::can::TxFaultLEDs{
        .imd = imd_fault,
        .bms = bms_fault,
    });

    bool fault = bms_fault || imd_fault;

    uint32_t time_ms = bindings::GetTick();

    if (fault) {
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

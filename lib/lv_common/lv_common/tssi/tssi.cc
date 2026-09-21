#include "tssi.hpp"

namespace tssi {

void Controller::SetEnabled(bool enable) {
    enabled = enable;
}

bool Controller::GetImdFault(void) {
    return imd_fault;
}

bool Controller::GetBmsFault(void) {
    return bms_fault;
}

/// @brief See FSAE 2025 Rules EV.5.11.5
void Controller::task_10hz(int time_ms) {
    periph.tssi_en->Set(enabled);

    imd_fault = periph.imd_fault->Read();
    bms_fault = periph.bms_fault->Read();

    // uint32_t time_ms = bindings::GetTick();

    if (bms_fault || imd_fault) {
        periph.tssi_green_signal->SetLow();
        periph.tssi_red_signal->Set(flash_toggle);
        if ((time_ms - state_entered_time) > kTogglePeriodMs) {
            state_entered_time = time_ms;
            flash_toggle = !flash_toggle;
        }
    } else {
        periph.tssi_red_signal->SetLow();
        periph.tssi_green_signal->SetHigh();
        flash_toggle = true;
    }
}

}  // namespace tssi

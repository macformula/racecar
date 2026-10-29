#include "tssi.hpp"

namespace macfe::lv {

void TSSI::SetEnabled(bool enable) {
    enabled = enable;
}

bool TSSI::GetImdFault(void) {
    return imd_fault;
}

bool TSSI::GetBmsFault(void) {
    return bms_fault;
}

/// @brief See FSAE 2025 Rules EV.5.11.5
void TSSI::task_10hz() {
    _tssi_en.Set(enabled);

    imd_fault = _imd_fault.Read();
    bms_fault = _bms_fault.Read();

    if (bms_fault || imd_fault) {
        _tssi_green_signal.SetLow();
        _tssi_red_signal.Set(flash_toggle);
        if ((time_in_state_ms) > kTogglePeriodMs) {
            time_in_state_ms = 0;
            flash_toggle = !flash_toggle;
        }
    } else {
        _tssi_red_signal.SetLow();
        _tssi_green_signal.SetHigh();
        flash_toggle = true;
    }
    time_in_state_ms += 100;
}

}  // namespace macfe::lv

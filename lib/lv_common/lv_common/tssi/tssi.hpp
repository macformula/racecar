#pragma once

#include "periph/gpio.hpp"
/// @brief Tractive System Active Indicator
/// A light on the car that indicates if the car can drive or is faulted.
namespace macfe::lv {

class TSSI {
public:
    TSSI(macfe::periph::DigitalOutput& tssi_en,
         macfe::periph::DigitalOutput& tssi_red_signal,
         macfe::periph::DigitalOutput& tssi_green_signal,
         macfe::periph::DigitalInput& imd_fault,
         macfe::periph::DigitalInput& bms_fault)
        : _tssi_en(tssi_en),
          _tssi_red_signal(tssi_red_signal),
          _tssi_green_signal(tssi_green_signal),
          _imd_fault(imd_fault),
          _bms_fault(bms_fault) {};

    void task_10hz();
    bool GetImdFault(void);
    bool GetBmsFault(void);

private:
    void SetEnabled(bool enable);

    macfe::periph::DigitalOutput& _tssi_en;
    macfe::periph::DigitalOutput& _tssi_red_signal;
    macfe::periph::DigitalOutput& _tssi_green_signal;
    macfe::periph::DigitalInput& _imd_fault;
    macfe::periph::DigitalInput& _bms_fault;
    const float kFlashFrequencyHz = 4.0f;  // must be between 2Hz - 5 Hz
    // /2 since one period is two toggles
    const int kTogglePeriodMs = (1000.f / kFlashFrequencyHz) / 2.f;

    bool flash_toggle = true;
    int time_in_state_ms;

    bool enabled = false;
    bool imd_fault;
    bool bms_fault;
};

}  // namespace macfe::lv

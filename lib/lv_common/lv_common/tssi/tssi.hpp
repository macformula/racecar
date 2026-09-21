#pragma once

#include "periph/gpio.hpp"
/// @brief Tractive System Active Indicator
/// A light on the car that indicates if the car can drive or is faulted.
namespace tssi {

struct tssi_periph {
    macfe::periph::DigitalOutput* tssi_en;
    macfe::periph::DigitalOutput* tssi_red_signal;
    macfe::periph::DigitalOutput* tssi_green_signal;
    macfe::periph::DigitalInput* imd_fault;
    macfe::periph::DigitalInput* bms_fault;
};
class Controller {
public:
    Controller(tssi_periph tssi_periph) : periph(tssi_periph) {};

    void task_10hz(int time_ms);

private:
    void SetEnabled(bool enable);

    bool GetImdFault(void);
    bool GetBmsFault(void);
    tssi_periph periph;
    const float kFlashFrequencyHz = 4.0f;  // must be between 2Hz - 5 Hz
    // /2 since one period is two toggles
    const int kTogglePeriodMs = (1000.f / kFlashFrequencyHz) / 2.f;

    bool flash_toggle = true;
    uint32_t state_entered_time = 0;

    bool enabled = false;
    bool imd_fault;
    bool bms_fault;
};

}  // namespace tssi

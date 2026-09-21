/// @author Manush Patel
/// @date 2026-05-24

#pragma once

#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace dcdc {
struct dcdc_periph {
    macfe::periph::DigitalOutput* vicor_en;
    macfe::periph::AnalogInput* bus_current;
    macfe::periph::AnalogInput* bus_voltage;
    macfe::periph::AnalogInput* lv_battery;
};

class Controller {
public:
    Controller(dcdc_periph dcdc_periph) : periph(dcdc_periph) {};
    void SetEnabled(bool enable);

    float GetVoltage(void);
    float GetLvBatteryVoltage(void);
    float GetAmps(void);

    void task_100hz(void);

private:
    void MeasureLvBatteryVoltage() {}
    void MeasureAmps();
    void MeasureVolts();
    dcdc_periph periph;
    bool enabled = false;
    float voltage = 0;
    float amps = 0;
    float lv_battery_voltage = 0;
};

}  // namespace dcdc

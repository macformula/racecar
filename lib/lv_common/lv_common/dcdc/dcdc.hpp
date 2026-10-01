/// @author Manush Patel
/// @date 2026-05-24

#pragma once

#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace macfe::lv {

class DCDC {
public:
    DCDC(macfe::periph::DigitalOutput& vicor_en,
         macfe::periph::AnalogInput& bus_current,
         macfe::periph::AnalogInput& bus_voltage,
         macfe::periph::AnalogInput& lv_battery)
        : _vicor_en(vicor_en),
          _bus_current(bus_current),
          _bus_voltage(bus_voltage),
          _lv_battery(lv_battery) {};
    void SetEnabled(bool enable);

    float GetVoltage(void);
    float GetLvBatteryVoltage(void);
    float GetAmps(void);

    void task_100hz(void);

private:
    void MeasureLvBatteryVoltage() {}
    void MeasureAmps();
    void MeasureVolts();
    macfe::periph::DigitalOutput& _vicor_en;
    macfe::periph::AnalogInput& _bus_current;
    macfe::periph::AnalogInput& _bus_voltage;
    macfe::periph::AnalogInput& _lv_battery;
    bool enabled = false;
    float voltage = 0;
    float amps = 0;
    float lv_battery_voltage = 0;
};

}  // namespace macfe::lv

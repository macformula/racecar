/// @author Manush Patel
/// @date 2026-05-24

#include "dcdc.hpp"

#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"
// new update in rev 4: no dcdc select, so this should go. Don't want to remove
// it yet, will make a separate issue.
namespace macfe::lv {

void DCDC::SetEnabled(bool enable) {
    enabled = enable;
}

float DCDC::GetVoltage(void) {
    return voltage;
}

float DCDC::GetLvBatteryVoltage(void) {
    return lv_battery_voltage;
}

float DCDC::GetAmps(void) {
    return amps;
}

void DCDC::MeasureAmps() {
    amps = _bus_current.ReadVoltage() * 5.0f;
}

void DCDC::MeasureVolts() {
    voltage = _bus_voltage.ReadVoltage() * 8.0f;
}

void DCDC::MeasureLvBatteryVoltage() {
    lv_battery_voltage = _lv_battery.ReadVoltage() * 10.0f;
}

void DCDC::task_100hz(void) {
    _vicor_en.Set(!enabled);
    MeasureAmps();
    MeasureVolts();
    MeasureLvBatteryVoltage();
}

}  // namespace macfe::lv
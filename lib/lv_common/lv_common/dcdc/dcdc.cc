/// @author Manush Patel
/// @date 2026-05-24

#include "dcdc.hpp"

#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"
// new update in rev 4: no dcdc select
namespace dcdc {

using macfe::periph::AnalogInput;
using macfe::periph::DigitalOutput;

void Controller::SetEnabled(bool enable) {
    enabled = enable;
}

float Controller::GetVoltage(void) {
    return voltage;
}

float Controller::GetLvBatteryVoltage(void) {
    return lv_battery_voltage;
}

float Controller::GetAmps(void) {
    return amps;
}

// TODO
// - Do we need can messages for lvbattery, current & voltage (should we have go
// to dash, rpi)
// - are these conversions 100% correct
// - is any time delay needed between select and read?

void Controller::MeasureAmps() {
    amps = periph.bus_current->ReadVoltage() * 5.0f;
}

void Controller::MeasureVolts() {
    voltage = periph.bus_voltage->ReadVoltage() * 8.0f;
}

void Controller::MeasureLvBatteryVoltage() {
    lv_battery_voltage = periph.lv_battery->ReadVoltage() * 10.0f;
}

void Controller::task_100hz(void) {
    periph.vicor_en->Set(!enabled);
    MeasureAmps();
    MeasureVolts();
    MeasureLvBatteryVoltage();
}

}  // namespace dcdc
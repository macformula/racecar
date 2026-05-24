/// @author Manush Patel
/// @date 2026-05-24

#include "dcdc.hpp"

#include "bindings.hpp"

// new update in rev 4: no dcdc select
namespace dcdc {

static bool enabled = false;
static float voltage = 0;
static float amps = 0;
static float lv_battery_voltage = 0;

void SetEnabled(bool enable) {
    enabled = enable;
}

float GetVoltage(void) {
    return voltage;
}

float GetLvBatteryVoltage(void) {
    return lv_battery_voltage;
}

float GetAmps(void) {
    return amps;
}

// TODO
// - Do we need can messages for lvbattery, current & voltage (should we have go
// to dash, rpi)
// - are these conversions 100% correct
// - is any time delay needed between select and read?

float MeasureLvBatteryVoltage(void) {
    return bindings::lv_battery.ReadVoltage();
}

static void MeasureAmps(void) {
    amps = bindings::bus_current.ReadVoltage() * 5.0f;
}

static void MeasureVolts(void) {
    voltage = bindings::bus_voltage.ReadVoltage();
}

void task_100hz() {
    bindings::vicor_en.Set(!enabled);
    MeasureAmps();
    MeasureVolts();
    MeasureLvBatteryVoltage();
}

}  // namespace dcdc

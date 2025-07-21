#include "dcdc.hpp"

#include "bindings.hpp"

namespace dcdc {

static bool enabled = false;
static float voltage = 0;
static float amps = 0;

void SetEnabled(bool enable) {
    enabled = enable;
}

float GetVoltage(void) {
    return voltage;
}

float GetAmps(void) {
    return amps;
}

// TODO
// - Check if V or I is sel=high/low
// - Convert adc voltage to physical value
// - is any time delay needed between select and read?

static void MeasureAmps(void) {
    bindings::dcdc_sense_select.SetHigh();
    amps = bindings::dcdc_sense.ReadVoltage();
}

static void MeasureVolts(void) {
    bindings::dcdc_sense_select.SetLow();
    voltage = bindings::dcdc_sense.ReadVoltage();
}

void task_100hz() {
    bindings::dcdc_en.Set(!enabled);  // active low

    MeasureAmps();
    MeasureVolts();
}

}  // namespace dcdc

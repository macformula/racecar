#include "dcdc.hpp"

#include "shared/periph/gpio.hpp"

DCDC::DCDC(shared::periph::DigitalOutput& enable_output_inverted,
           shared::periph::DigitalOutput& select_sense,
           shared::periph::ADCInput& sense)
    : enable_output_inverted_(enable_output_inverted),
      select_sense_(select_sense),
      sense_(sense) {}

void DCDC::Enable() {
    enable_output_inverted_.SetLow();
}
void DCDC::Disable() {
    enable_output_inverted_.SetHigh();
}

// TODO
// - Check if V or I is sel=high/low
// - Convert adc to value
// - is any time delay needed between select and read?
float DCDC::MeasureAmps() {
    select_sense_.SetHigh();
    return sense_.Read();
}

float DCDC::MeasureVoltage() {
    select_sense_.SetLow();
    return sense_.Read();
}
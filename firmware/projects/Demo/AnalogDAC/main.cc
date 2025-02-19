#include "bindings.hpp"

using namespace bindings;

/**
 * @brief Tests the AnalogOutputDAC class for the stm32 implementation
 */
int main(void) {
    Initialize();

    while (true) {
        analog_out_dac.SetVoltage(1.0f);
        DelayMS(1000);
        analog_out_dac.SetVoltage(4.5f);
        DelayMS(1000);
        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);
        analog_out_dac.SetVoltage(3.3f);
        DelayMS(1000);
    }

    return 0;
}

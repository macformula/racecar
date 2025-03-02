#include "bindings.hpp"
#include <math.h>
#include "../../../shared/util/mappers/clamper.hpp"

#define STEP_SIZE 0.1f
#define MAX_VOLTAGE 2.5f
#define MIN_VOLTAGE 1.0f
#define DELAY 50
#define OSCILLATIONS 5

using namespace bindings;

/**
* @brief Tests the AnalogOutputDAC class using a square wave.
*/
void DemoSquareWave(float minVoltage, float maxVoltage) {
    analog_out_dac.SetVoltage(minVoltage);
    DelayMS(DELAY * 10);
    analog_out_dac.SetVoltage(maxVoltage);
    DelayMS(DELAY * 10);
}


/**
* @brief Tests the AnalogOutputDAC class using a ramp.
*/
void DemoRamp(float minVoltage, float maxVoltage) {
    for (float v = minVoltage; v <= maxVoltage; v += STEP_SIZE) {
        analog_out_dac.SetVoltage(v);
        DelayMS(DELAY);
    }
}


/**
* @brief Tests the AnalogOutputDAC class using a sine wave.
*/
void DemoSineWave(float minVoltage, float maxVoltage) {
    float degreeStepSize = STEP_SIZE * 100;
    float amplitude = (maxVoltage - minVoltage) / 2;
    float offset = (maxVoltage + minVoltage) / 2;

    for (int i = 0; i < 360; i += degreeStepSize) {
        float voltage = (amplitude * sin(i * M_PI / 180.0f)) + offset;
        analog_out_dac.SetVoltage(voltage);
        DelayMS(DELAY);
    }
}


/**
* @brief Tests the AnalogOutputDAC class using a triangle wave.
*/
void DemoTriangleWave(float minVoltage, float maxVoltage) {
    for (float v = minVoltage; v <= maxVoltage; v += STEP_SIZE) {
        analog_out_dac.SetVoltage(v);
        DelayMS(DELAY);
    }
    for (float v = maxVoltage; v >= minVoltage; v -= STEP_SIZE) {
        analog_out_dac.SetVoltage(v);
        DelayMS(DELAY);
    }
}


/**
 * @brief Tests the AnalogOutputDAC class for the stm32 implementation
 */
int main(void) {
    Initialize();

    while (true) {

        // Clamp voltage
        float maxVoltage = shared::util::Clamper<float>::Evaluate(
            MAX_VOLTAGE, 0, 3.3);
        
        float minVoltage = shared::util::Clamper<float>::Evaluate(
            MIN_VOLTAGE, 0, maxVoltage);
        
        
        for (int i = 0; i < OSCILLATIONS; i++)
        {
            DemoSquareWave(minVoltage, maxVoltage);
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

        for (int i = 0; i < OSCILLATIONS; i++)
        {
            DemoRamp(minVoltage, maxVoltage);
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

        for (int i = 0; i < OSCILLATIONS; i++)
        {
            DemoSineWave(minVoltage, maxVoltage);
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

        for (int i = 0; i < OSCILLATIONS; i++)
        {
            DemoTriangleWave(minVoltage, maxVoltage);
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

    }

    return 0;
}

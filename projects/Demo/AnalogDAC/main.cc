#include <math.h>

#include "bindings.hpp"

const float STEP_SIZE = 0.1f;
const float MAX_VOLTAGE = 3.0f;
const float MIN_VOLTAGE = 0.0f;
const int DELAY_MS = 50;
const int OSCILLATIONS = 5;

using namespace bindings;

void DemoSquareWave() {
    analog_out_dac.SetVoltage(MIN_VOLTAGE);
    DelayMS(DELAY_MS * 10);
    analog_out_dac.SetVoltage(MAX_VOLTAGE);
    DelayMS(DELAY_MS * 10);
}

void DemoRamp() {
    for (float v = MIN_VOLTAGE; v <= MAX_VOLTAGE; v += STEP_SIZE) {
        analog_out_dac.SetVoltage(v);
        DelayMS(DELAY_MS);
    }
}

void DemoSineWave() {
    float degreeStepSize = STEP_SIZE * 100;
    float amplitude = (MAX_VOLTAGE - MIN_VOLTAGE) / 2;
    float offset = (MAX_VOLTAGE + MIN_VOLTAGE) / 2;

    for (int i = 0; i < 360; i += degreeStepSize) {
        float voltage = (amplitude * sin(i * M_PI / 180.0f)) + offset;
        analog_out_dac.SetVoltage(voltage);
        DelayMS(DELAY_MS);
    }
}

void DemoTriangleWave() {
    for (float v = MIN_VOLTAGE; v <= MAX_VOLTAGE; v += STEP_SIZE) {
        analog_out_dac.SetVoltage(v);
        DelayMS(DELAY_MS);
    }
    for (float v = MAX_VOLTAGE; v >= MIN_VOLTAGE; v -= STEP_SIZE) {
        analog_out_dac.SetVoltage(v);
        DelayMS(DELAY_MS);
    }
}

int main(void) {
    Initialize();

    // Cycle through each pattern
    while (true) {
        for (int i = 0; i < OSCILLATIONS; i++) {
            DemoSquareWave();
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

        for (int i = 0; i < OSCILLATIONS; i++) {
            DemoRamp();
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

        for (int i = 0; i < OSCILLATIONS; i++) {
            DemoSineWave();
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);

        for (int i = 0; i < OSCILLATIONS; i++) {
            DemoTriangleWave();
        }

        analog_out_dac.SetVoltage(0.0f);
        DelayMS(1000);
    }

    return 0;
}

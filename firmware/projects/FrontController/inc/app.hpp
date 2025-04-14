#pragma once

#include "etl/algorithm.h"
#include "shared/periph/analog_input.hpp"

class Pedal {
public:
    shared::periph::AnalogInput& sensor_;
    const float volt_released;
    const float volt_compressed;

    /// Convert a pedal sensor voltage to 0-100% position
    float ReadPosition() {
        float voltage = sensor_.ReadVoltage();
        float raw_pos = 100.f * (voltage - volt_released) /
                        (volt_compressed - volt_released);

        return etl::clamp<float>(raw_pos, 0, 100);
    }
};

class SteeringWheel {
public:
    shared::periph::AnalogInput& sensor_;
    const float volt_straight;
    const float volt_full_right;

    // Straight -> 0, Left -> -1, Right -> +1
    float ReadPosition() {
        float voltage = sensor_.ReadVoltage();
        float raw_pos =
            (voltage - volt_straight) / (volt_full_right - volt_straight);

        return etl::clamp<float>(raw_pos, -1, 1);
    }
};
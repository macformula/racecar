#pragma once

#include <cstddef>

#include "shared/periph/analog_input.hpp"
#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"

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
        return shared::util::Clamper<float>::Evaluate(raw_pos, 0, 100);
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

        return shared::util::Clamper<float>::Evaluate(raw_pos, -1, 1);
    }
};
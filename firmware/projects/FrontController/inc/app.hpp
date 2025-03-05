#pragma once

#include <cstddef>

#include "shared/periph/analog_input.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"

class AnalogInput {
    static constexpr size_t kMovingAverageLength = 20;

public:
    AnalogInput(shared::periph::AnalogInput& sensor,
                shared::util::Mapper<float>* volt_to_position)
        : sensor_(sensor), volt_to_position_(volt_to_position) {}

    float Update() {
        moving_average_.LoadValue(sensor_.ReadVoltage());
        return GetPosition();
    }

    inline float GetPosition() {
        return volt_to_position_->Evaluate(moving_average_.GetValue());
    }

private:
    shared::periph::AnalogInput& sensor_;
    shared::util::MovingAverage<kMovingAverageLength> moving_average_;
    const shared::util::Mapper<float>* volt_to_position_;
};

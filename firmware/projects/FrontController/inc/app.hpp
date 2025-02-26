#pragma once

#include <cstddef>

#include "shared/periph/analog_input.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"

class AnalogInput {
    static constexpr size_t kMovingAverageLength = 20;

public:
    AnalogInput(shared::periph::AnalogInput& adc,
                shared::util::Mapper<float>* adc_to_position)
        : adc_(adc), adc_to_position_(adc_to_position) {}

    float Update() {
        moving_average_.LoadValue(adc_.ReadVoltage());
        return GetPosition();
    }

    inline float GetPosition() {
        return adc_to_position_->Evaluate(moving_average_.GetValue());
    }

private:
    shared::periph::AnalogInput& adc_;
    shared::util::MovingAverage<kMovingAverageLength> moving_average_;
    const shared::util::Mapper<float>* adc_to_position_;
};

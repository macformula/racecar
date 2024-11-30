/// @author Blake Freer
/// @date 2024-02-27

#pragma once

#include <cstddef>
#include <cstdint>

#include "shared/periph/adc.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"

class AnalogInput {
    static constexpr size_t kMovingAverageLength = 20;

public:
    AnalogInput(shared::periph::ADCInput& adc,
                shared::util::Mapper<double, uint16_t>* adc_to_position)
        : adc_(adc), adc_to_position_(adc_to_position) {}

    double Update() {
        uint32_t position = adc_.Read();
        moving_average_.LoadValue(uint16_t(position));
        return GetPosition();
    }

    /**
     * @brief Get the position from the last `Update()` call.
     */
    inline double GetPosition() {
        return adc_to_position_->Evaluate(moving_average_.GetValue());
    }

private:
    shared::periph::ADCInput& adc_;
    shared::util::MovingAverage<uint16_t, kMovingAverageLength> moving_average_;
    const shared::util::Mapper<double, uint16_t>* adc_to_position_;
};

struct ContactorInput {
    bool Pack_Precharge_Feedback;
    bool Pack_Negative_Feedback;
    bool Pack_Positive_Feedback;
    bool HvilFeedback;
    int8_t LowThermValue;
    int8_t HighThermValue;
    int8_t AvgThermValue;
    double PackSOC;
};

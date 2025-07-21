#pragma once

#include "lookup_table.hpp"
#include "moving_average.hpp"
#include "periph/analog_input.hpp"

class TempSensor {
public:
    TempSensor(macfe::periph::AnalogInput& analog_input);

    float Update();

private:
    macfe::periph::AnalogInput& analog_input_;

    static constexpr int moving_average_length_ = 20;
    macfe::MovingAverage<moving_average_length_> rolling_temperature_;

    float Read();

    /// This table is directly copied from Table 4 of the temperature sensor
    /// datasheet.
    /// `datasheets/energus/Datasheet_with_VTC6_rev_A(2021-10-26).pdf`
    static constexpr auto volt_ts_to_degC =
        std::to_array<macfe::LookupTable::Entry>({
            // clang-format off
        {1.30f, 120.0f},
        {1.31f, 115.0f},
        {1.32f, 110.0f},
        {1.33f, 105.0f},
        {1.34f, 100.0f},
        {1.35f,  95.0f},
        {1.37f,  90.0f},
        {1.38f,  85.0f},
        {1.40f,  80.0f},
        {1.43f,  75.0f},
        {1.45f,  70.0f},
        {1.48f,  65.0f},
        {1.51f,  60.0f},
        {1.55f,  55.0f},
        {1.59f,  50.0f},
        {1.63f,  45.0f},
        {1.68f,  40.0f},
        {1.74f,  35.0f},
        {1.80f,  30.0f},
        {1.86f,  25.0f},
        {1.92f,  20.0f},
        {1.99f,  15.0f},
        {2.05f,  10.0f},
        {2.11f,   5.0f},
        {2.17f,   0.0f},
        {2.23f,  -5.0f},
        {2.27f, -10.0f},
        {2.32f, -15.0f},
        {2.35f, -20.0f},
        {2.38f, -25.0f},
        {2.40f, -30.0f},
        {2.42f, -35.0f},
        {2.44f, -40.0f},
            // clang-format on
        });
};

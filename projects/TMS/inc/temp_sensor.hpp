#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/util/lookup_table.hpp"
#include "shared/util/moving_average.hpp"

class TempSensor {
    using LUT = shared::util::LookupTable<float>;

public:
    TempSensor(shared::periph::AnalogInput& analog_input)
        : analog_input_(analog_input), rolling_temperature_() {}

    float Update() {
        float new_temperature = Read();
        rolling_temperature_.LoadValue(new_temperature);
        return rolling_temperature_.GetValue();
    }

private:
    shared::periph::AnalogInput& analog_input_;

    static constexpr int moving_average_length_ = 20;
    shared::util::MovingAverage<moving_average_length_> rolling_temperature_;

    float Read() {
        float volt_at_stm = analog_input_.ReadVoltage();

        /// Calculate the voltage at the temperature sensor from the voltage
        /// at the STM. They are not equal because there is a non-unity gain
        /// buffer between them. V_STM = 1.44 + 0.836 * V_TS / 2 So the
        /// inverse is V_TS = 2 * (V_STM - 1.44) / 0.83
        float volt_at_tempsensor = 2 * (volt_at_stm - 1.44) / 0.836;
        float temperature = LUT::Evaluate(volt_ts_to_degC, volt_at_tempsensor);

        return temperature;
    }

    /// This table is directly copied from Table 4 of the temperature sensor
    /// datasheet.
    /// `datasheets/energus/Datasheet_with_VTC6_rev_A(2021-10-26).pdf`
    static constexpr auto volt_ts_to_degC = std::to_array<LUT::Entry>({
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

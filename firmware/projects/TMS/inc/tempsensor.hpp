#pragma once

#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/lookup_table.hpp"
#include "shared/util/mappers/mapper.hpp"

namespace tempsensor {

/// This table is directly copied from Table 4 of the temperature sensor
/// datasheet. `datasheets/energus/Datasheet_with_VTC6_rev_A(2021-10-26).pdf`
auto ts_table = std::to_array<shared::util::LookupTable<float>::Entry>({
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
const shared::util::LookupTable volt_ts_to_degC{ts_table};

/// Calculate the voltage at the temperature sensor from the voltage at the STM.
/// They are not equal because there is a non-unity gain buffer between them.
/// V_STM = 1.44 + 0.836 * V_TS / 2
/// So the inverse is
/// V_TS = 2 * (V_STM - 1.44) / 0.836 = 2/0.836 * V_STM - 2*1.44/0.836
const shared::util::LinearMap<float> volt_stm_to_volt_ts{
    2.0f / 0.836f,
    -2.0f * 1.44f / 0.836f,
};

/// Compose the two maps to get the final map from the STM voltage to the
/// temperature in degrees C.
const shared::util::CompositeMap<float> volt_stm_to_degC{
    volt_stm_to_volt_ts,  // inner (first) function
    volt_ts_to_degC,      // outer (second) function
};

}  // namespace tempsensor
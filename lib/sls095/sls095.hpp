/// @author Manush Patel
/// @date 2025-10

/*
 https://www.cw-industrial.com/sites/default/files/home/industrial-sensors/linear-position/SLS095-Technical-Information.pdf
*/

#pragma once

namespace macfe {

inline constexpr float STROKE_MM = 75.0f;

float VoltToMillimeter(float input_voltage, float supply_voltage) {
    if (supply_voltage <= 0) {
        return 0.0f;
    }

    return STROKE_MM * input_voltage / supply_voltage;  //
}

};  // namespace macfe

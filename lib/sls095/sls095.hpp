/// @author Manush Patel
/// @date 2025-10

#pragma once

#include <span>
#include <type_traits>



namespace macfe {

    inline constexpr float stroke_mm    = 75.0f;  // Physical stroke length in millimeters

    float positionFromVoltage(float inputVoltage, float supplyVoltage) {

        if (supplyVoltage <= 0 ){ 
            return 0.0f;
        }
        
        return stroke_mm * inputVoltage / supplyVoltage; //
    }
};



#pragma once
#include <cmath>

// everson shen and aleeza ali zar

namespace ctrl {

template <typename T>
bool isInRange(T fractionIn) {
    return (fractionIn >= 0 && fractionIn <= 1);
}

template <typename T>
bool b_DriverInterfaceError(T DI_V_AccelPedalPos1, T DI_V_AccelPedalPos2,
                            T DI_FrontBrakePressure, T DI_RearBrakePressure,
                            T DI_V_RawSteeringAngle) {
    return (
        !isInRange(DI_V_AccelPedalPos1) || !isInRange(DI_V_AccelPedalPos2) ||
        !isInRange(DI_FrontBrakePressure) || !isInRange(DI_RearBrakePressure) ||
        !isInRange(DI_V_RawSteeringAngle) ||
        (std::abs(DI_V_AccelPedalPos1 - DI_V_AccelPedalPos2) > 0.1));
}
}  // namespace ctrl

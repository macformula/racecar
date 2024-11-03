#pragma once
#include <algorithm>
#include <iostream>
#include <tuple>

#include "../util/mappers/lookup_table.h"

namespace ctrl {

// Brake Pedal Map Function
template <typename T>
T brakePedalMap(T input) {
    T lookupTableData[][2] = {
        {0, 0},   {5, 5},   {10, 10}, {15, 15}, {20, 20}, {25, 25}, {30, 30},
        {35, 35}, {40, 40}, {45, 45}, {50, 50}, {55, 55}, {60, 60}, {65, 65},
        {70, 70}, {75, 75}, {80, 80}, {85, 85}, {90, 90}, {95, 95}, {100, 100}};

    constexpr int size = (sizeof(lookupTableData) / sizeof(lookupTableData[0]));

    shared::util::LookupTable<size> brakePedalMap(lookupTableData);

    return brakePedalMap.Evaluate(input);
}

template <typename T>
std::tuple<T, bool> brake_pedal_lights(bool rearBrakePedalError,
                                       bool frontBrakePedalError,
                                       T rearBrakePedalPos,
                                       T frontBrakePedalPos) {
    T brakePedalPos;

    // Brake Error Handling
    if (rearBrakePedalError || frontBrakePedalError) {
        brakePedalPos = 0;
    } else {
        T brakeInput = std::max(frontBrakePedalPos, rearBrakePedalPos);

        // Brake Pedal Mapping
        brakePedalPos = brakePedalMap(brakeInput);
    }

    // Turn brake light on
    bool brakeLightEn = (brakePedalPos > 0.1);

    return std::tuple(brakePedalPos, brakeLightEn);
}

}  // namespace ctrl
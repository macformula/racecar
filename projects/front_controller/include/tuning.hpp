#pragma once

#include <array>

#include "generated/can/veh_messages.hpp"
#include "lookup_table.hpp"

namespace tuning {

struct Profile {
    // SAM: don't worry about what these parameters mean for now
    float aggressiveness;
    float damping;
};

/***************************************************************
    DRIVER PARAMETERS
***************************************************************/

// Map a [0%, 100%] pedal position to [0%, 100%] torque request.
// Linearly interpolated between keypoints.
constexpr auto pedal_to_torque = std::to_array<macfe::LookupTable::Entry>({
    // Syntax: {pedal pos, torque}. Add more rows as desired.
    // Must be sorted by increasing pedal position.
    {0.0f, 0.0f},
    {100.f, 100.f},
});

constexpr Profile GetProfile(generated::can::RxDashStatus::Profile_t profile) {
    using enum generated::can::RxDashStatus::Profile_t;
    switch (profile) {
        case Default:
            return {.aggressiveness = 50, .damping = 50};

        case Launch:
            return {.aggressiveness = 90, .damping = 10};

        case Skidpad:
            return {.aggressiveness = 50, .damping = 30};

        case Endurance:
            return {.aggressiveness = 30, .damping = 50};

        case _ENUM_TAIL_:
        default:
            // Shouldn't ever be called in this situation. Return Default to be
            // safe
            return GetProfile(Default);
    }
}

}  // namespace tuning
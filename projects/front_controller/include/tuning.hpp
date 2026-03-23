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
constexpr auto pedal_to_torque_linear =
    std::to_array<macfe::LookupTable::Entry>({
        // Syntax: {pedal pos, torque}. Add more rows as desired.
        // Must be sorted by increasing pedal position.
        {0.0f, 0.0f},
        {100.f, 100.f},
    });

// Same as above, but with a Quadratic Curve instead of Linear (torque =
// pedal^2/100)
constexpr auto pedal_to_torque_quadratic =
    std::to_array<macfe::LookupTable::Entry>({
        {0.f, 0.f},
        {25.f, 6.25f},
        {50.f, 25.f},
        {75.f, 56.25f},
        {100.f, 100.f},
    });

// The type of pedal map to use. Can be switched in the system
enum class PedalMapType {
    Linear,
    Quadratic,
};

// The active pedal map to use. Change this to switch between Quadratic and
// Linear maps
constexpr PedalMapType active_pedal_map = PedalMapType::Quadratic;

// Get the active pedal map based on the value of active_pedal_map
constexpr const auto& get_pedal_map() {
    if constexpr (active_pedal_map == PedalMapType::Linear) {
        return pedal_to_torque_linear;
    } else {
        return pedal_to_torque_quadratic;
    }
}

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
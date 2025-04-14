#include <array>

#include "shared/util/mappers/lookup_table.hpp"

namespace tuning {

/***************************************************************
    DRIVER PARAMETERS
***************************************************************/

// Map a [0%, 100%] pedal position to [0%, 100%] torque request.
// Linearly interpolated between keypoints.
constexpr auto pedal_to_torque =
    std::to_array<shared::util::LookupTable<float>::Entry>({
        // Syntax: {pedal pos, torque}. Add more rows as desired.
        // Must be sorted by increasing pedal position.
        {0.0f, 0.0f},
        {100.f, 100.f},
    });

}  // namespace tuning
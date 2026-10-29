#pragma once

#include "periph/analog_input.hpp"

namespace sensors::dynamics {

struct WheelSpeed {
    float rear_left;
    float rear_right;
    float front_left;
    float front_right;
};

const WheelSpeed& GetWheelSpeeds(void);

void Update_100Hz(macfe::periph::AnalogInput& wheel_speed_front_left,
                  macfe::periph::AnalogInput& wheel_speed_front_right,
                  macfe::periph::AnalogInput& wheel_speed_rear_left,
                  macfe::periph::AnalogInput& wheel_speed_rear_right);

}  // namespace sensors::dynamics

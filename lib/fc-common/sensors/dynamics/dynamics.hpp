#pragma once

namespace sensors::dynamics {

struct WheelSpeed {
    float rear_left;
    float rear_right;
    float front_left;
    float front_right;
};

const WheelSpeed& GetWheelSpeeds(void);

void Update_100Hz(void);

}  // namespace sensors::dynamics
#include "dynamics.hpp"

#include "bindings.hpp"

namespace sensors::dynamics {

static WheelSpeed wheel_speed = {0, 0, 0, 0};

const WheelSpeed& GetWheelSpeeds(void) {
    return wheel_speed;
}

static float VoltageToRpm(float voltage) {
    (void)voltage;
    return 0;  // todo. are they analog sensors or tachometers?
}

void Update_100Hz(void) {
    wheel_speed.front_left =
        VoltageToRpm(bindings::wheel_speed_front_left.ReadVoltage());
    wheel_speed.front_right =
        VoltageToRpm(bindings::wheel_speed_front_right.ReadVoltage());
    wheel_speed.rear_left =
        VoltageToRpm(bindings::wheel_speed_rear_left.ReadVoltage());
    wheel_speed.rear_right =
        VoltageToRpm(bindings::wheel_speed_rear_right.ReadVoltage());
}

}  // namespace sensors::dynamics
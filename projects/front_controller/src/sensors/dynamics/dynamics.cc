#include "dynamics.hpp"

#include "bindings.hpp"

namespace sensors::dynamics {

const int32_t TICKS_PER_REVOLUTION = 30;  // the wheel has 30 ticks on it

int32_t prev_right_wheel_tick_count = 0;
int32_t prev_left_wheel_tick_count = 0;
float right_wheel_speed_rpm = 0;
float left_wheel_speed_rpm = 0;

void Update_10Hz(void) {
    // calculate right wheel speed
    int32_t cur_right_wheel_tick_count = bindings::GetRightWheelTick();

    right_wheel_speed_rpm =
        (float)(cur_right_wheel_tick_count - prev_right_wheel_tick_count) *
        10.f * 60.f / TICKS_PER_REVOLUTION;
    prev_right_wheel_tick_count = cur_right_wheel_tick_count;

    // calculate left wheel speed
    int32_t cur_left_wheel_tick_count = bindings::GetLeftWheelTick();

    left_wheel_speed_rpm =
        (float)(cur_left_wheel_tick_count - prev_left_wheel_tick_count) * 10.f *
        60.f / TICKS_PER_REVOLUTION;
    prev_left_wheel_tick_count = cur_left_wheel_tick_count;
}

float GetRightWheelRPM() {
    return right_wheel_speed_rpm;
}

float GetLeftWheelRPM() {
    return left_wheel_speed_rpm;
}

}  // namespace sensors::dynamics
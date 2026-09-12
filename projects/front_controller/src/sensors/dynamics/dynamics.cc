#include "dynamics.hpp"

#include <cstdint>

#include "bindings.hpp"
#include "physical.hpp"

namespace sensors::dynamics {

class Tachometer {
public:
    float Update(uint32_t current_ticks, float dt_sec) {
        uint32_t delta = current_ticks - prev_ticks_;
        prev_ticks_ = current_ticks;
        return (static_cast<float>(delta) / tuning::wheel_teeth_count) *
               (60.0f / dt_sec);
    }

private:
    uint32_t prev_ticks_ = 0;
};

static WheelSpeed wheel_speed = {0, 0, 0, 0};
static Tachometer tach_front_left;
static Tachometer tach_front_right;

const WheelSpeed& GetWheelSpeeds(void) {
    return wheel_speed;
}

float GetMph(void) {
    constexpr float PI = 3.1415926f;
    constexpr float in_per_rev = PI * tuning::motor_diam_inch;
    constexpr float in_per_mile = 12.0f * 5280.0f;
    constexpr float min_per_hr = 60.0f;

    float avg_rpm = (wheel_speed.front_left + wheel_speed.front_right) / 2.0f;
    return (avg_rpm * in_per_rev * min_per_hr) / in_per_mile;
}

void Update_100Hz(void) {
    constexpr float kDtSeconds = 0.01f;
    wheel_speed.front_left =
        tach_front_left.Update(bindings::GetWheelTicksLeft(), kDtSeconds);
    wheel_speed.front_right =
        tach_front_right.Update(bindings::GetWheelTicksRight(), kDtSeconds);
}

}  // namespace sensors::dynamics
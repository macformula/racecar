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

void Update_100Hz(void) {
    constexpr float kDtSeconds = 0.01f;
    wheel_speed.front_left =
        tach_front_left.Update(bindings::GetWheelTicksLeft(), kDtSeconds);
    wheel_speed.front_right =
        tach_front_right.Update(bindings::GetWheelTicksRight(), kDtSeconds);
}

}  // namespace sensors::dynamics
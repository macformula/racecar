#include "vehicle_dynamics_calc.hpp"

#include <algorithm>

#include "etl/algorithm.h"
#include "lookup_table.hpp"
#include "sensors/dynamics/dynamics.hpp"

namespace ctrl {

float CreateTorqueVectoringFactor(float steering_angle) {
    float absolute_steering_angle = std::abs(steering_angle);

    auto tv_lut = std::to_array<macfe::LookupTable::Entry>({
        {0.0, 1.0f},
        {5.0, 0.934f},
        {10.0, 0.87f},
        {15.0, 0.808f},
        {20.0, 0.747f},
        {25.0, 0.683f},
    });

    return macfe::LookupTable::Evaluate(tv_lut, absolute_steering_angle);
}

TorqueVector AdjustTorqueVectoring(float steering_angle) {
    float torque_vectoring_factor = CreateTorqueVectoringFactor(steering_angle);

    if (steering_angle > 0) {
        return {.left = 1.f, .right = torque_vectoring_factor};
    } else if (steering_angle < 0) {
        return {.left = torque_vectoring_factor, .right = 1.f};
    } else {
        return {.left = 1.f, .right = 1.f};
    }
}

float CalculateActualSlip(const sensors::dynamics::WheelSpeed& ws) {
    constexpr float kMinIdleSpeedRpm = 21.0f;  // ~1 mph with 16" wheels
    float idle_wheel_spd = (ws.front_left + ws.front_right) / 2.0f;

    if (idle_wheel_spd < kMinIdleSpeedRpm) {
        return 0.0f;
    }

    float actual_slip =
        std::max(ws.rear_left, ws.rear_right) / idle_wheel_spd - 1.0f;

    if (actual_slip < 0.0f) {
        actual_slip = 0.0f;
    }
    return actual_slip;
}

void TractionControl::Init(int time_ms) {
    is_slipping_.Init(time_ms, false);
    reset_time_ = time_ms - kRampTimeMs;
}

float TractionControl::UpdateScaleFactor(float actual_slip, float target_slip,
                                         int time_ms) {
    is_slipping_.Update(actual_slip > target_slip, time_ms);

    float scale_factor;
    if (is_slipping_.HeldForMs(true) >= 50) {
        // Disable torque so wheels can "catch up" and stop slipping
        scale_factor = 0;
        reset_time_ = time_ms;
    } else {
        // Ramp up to full torque
        scale_factor = static_cast<float>(time_ms - reset_time_) / kRampTimeMs;
        scale_factor = etl::clamp<float>(scale_factor, 0, 1);
    }
    // This method still isn't float ideal. It will continue ramping for up to
    // 49 msec of slipping. We should probably have a smarter algorithm that
    // predicts slipping before it happens, or at least is cautious when it
    // starts slipping.

    return scale_factor;
}

void HoldCondition::Init(int time_ms, bool initial_state) {
    reset_time_ = time_ms;
    current_state_ = initial_state;
}

void HoldCondition::Update(bool state, int time_ms) {
    if (state != current_state_) {
        reset_time_ = time_ms;
    }
    current_time_ = time_ms;
    current_state_ = state;
}

int HoldCondition::HeldForMs(bool state) const {
    if (current_state_ == state) {
        return current_time_ - reset_time_;
    } else {
        return 0;
    }
}

}  // namespace ctrl

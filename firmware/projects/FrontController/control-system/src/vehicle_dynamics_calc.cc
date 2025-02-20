#include "control-system/vehicle_dynamics_calc.hpp"

#include <algorithm>

#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/lookup_table.hpp"

namespace ctrl {

float TorqueRequest::Update(float driver_torque_request,
                            float brake_pedal_position) {
    bool brake_on = brake_pedal_position > 10.f;

    bool both_pedals_pressed = (driver_torque_request >= 25.f && brake_on);
    bool neither_pedal_pressed = driver_torque_request < 5.f && !brake_on;

    if (both_pedals_pressed) {
        current_state_ = State::Stop;
    }
    if (neither_pedal_pressed) {
        current_state_ = State::Run;
    }

    float driver_torque;

    switch (current_state_) {
        case State::Run:
            driver_torque = driver_torque_request;
            break;
        case State::Stop:
            driver_torque = 0.f;
            break;
    }

    return driver_torque;
}

float CreateTorqueVectoringFactor(float steering_angle) {
    float absolute_steering_angle = std::abs(steering_angle);

    static const float table_data[][2] = {{0.0, 1.0f},    {5.0, 0.934f},
                                          {10.0, 0.87f},  {15.0, 0.808f},
                                          {20.0, 0.747f}, {25.0, 0.683f}};

    static constexpr int table_length =
        (sizeof(table_data)) / (sizeof(table_data[0]));

    static shared::util::LookupTable<table_length> tv_lookup_table{table_data};

    return tv_lookup_table.Evaluate(absolute_steering_angle);
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

// Note: The CalculateActualSlip function has Div-by-Zero error if left front
// and right front wheel speeds = 0.
float CalculateActualSlip(float left_rear_wheel_speed,
                          float right_rear_wheel_speed,
                          float left_front_wheel_speed,
                          float right_front_wheel_speed) {
    float idle_wheel_spd =
        (left_front_wheel_speed + right_front_wheel_speed) / 2.0;
    float actual_slip =
        std::max(left_rear_wheel_speed, right_rear_wheel_speed) /
            idle_wheel_spd -
        1;

    if (actual_slip < 0) {
        actual_slip = 0;
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
        scale_factor =
            shared::util::Clamper<float>::Evaluate(scale_factor, 0, 1);
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
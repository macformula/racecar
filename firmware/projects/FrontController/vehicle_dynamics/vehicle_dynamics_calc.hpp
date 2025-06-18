#pragma once

#include "sensors/dynamics/dynamics.hpp"

namespace ctrl {

struct TorqueVector {
    float left;
    float right;
};

float CreateTorqueVectoringFactor(float steering_angle);

TorqueVector AdjustTorqueVectoring(float steering_angle);

float CalculateActualSlip(const sensors::dynamics::WheelSpeed& wheel_speed);

/// @brief Measures the time for which a boolean condition has been continuously
/// held.
class HoldCondition {
public:
    void Init(int time_ms, bool initial_state);

    void Update(bool state, int time_ms);

    /// @brief Return the number of milliseconds for which `state` has been
    /// continuously held.
    /// @note Returns 0 if `state` does not match the most recent state from
    /// `.Update(state, ...)`
    int HeldForMs(bool state) const;

private:
    int reset_time_;
    int current_time_;
    bool current_state_;
};

class TractionControl {
public:
    void Init(int time_ms);
    float UpdateScaleFactor(float actual_slip, float target_slip, int time_ms);

private:
    constexpr static float kRampTimeMs = 100.f;
    HoldCondition is_slipping_;
    int reset_time_;
};

}  // namespace ctrl

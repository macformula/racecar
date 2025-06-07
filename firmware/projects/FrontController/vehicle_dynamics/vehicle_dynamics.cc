/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "vehicle_dynamics.hpp"

#include "bindings.hpp"
#include "generated/can/veh_messages.hpp"
#include "motors/motors.hpp"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "shared/util/lookup_table.hpp"
#include "shared/util/moving_average.hpp"
#include "tuning.hpp"
#include "vehicle_dynamics_calc.hpp"

using namespace ctrl;
using namespace generated::can;
using Profile_t = RxDashboardStatus::Profile_t;
using LUT = shared::util::LookupTable<float>;
namespace vehicle_dynamics {

static motors::Request left_request;
static motors::Request right_request;

static Profile_t profile;
static float target_slip_ratio;
static TractionControl traction_ctrl;
static TorqueRequest torque_request;
static bool torque_vector_enable;

static shared::util::MovingAverage<10, float> torque_ma{};

amk::Request GetLeftMotorRequest(void) {
    return left_request;
}

amk::Request GetRightMotorRequest(void) {
    return right_request;
}

void SetProfile(generated::can::RxDashboardStatus::Profile_t _profile) {
    profile = _profile;
}

void SetTorqueVectorEnable(bool enable) {
    torque_vector_enable = enable;
}

void SetTargetSlipRatio(float target_slip) {
    target_slip_ratio = target_slip;
}

void Init(void) {
    SetTorqueVectorEnable(false);
    SetTargetSlipRatio(0.2f);
    SetProfile(Profile_t::Default);

    // Fix speed request and control the torque limits
    left_request = {
        .speed_request = 1000.f,
        .torque_limit_positive = 0,
        .torque_limit_negative = 0,
    };
    right_request = {
        .speed_request = 1000.f,
        .torque_limit_positive = 0,
        .torque_limit_negative = 0,
    };

    traction_ctrl.Init(bindings::GetTickMs());
}

void Update_100Hz(float driver_torque_request) {
    int time_ms = bindings::GetTickMs();

    float actual_slip =
        CalculateActualSlip(sensors::dynamics::GetWheelSpeeds());

    float tc_scale_factor = traction_ctrl.UpdateScaleFactor(
        actual_slip, target_slip_ratio, time_ms);

    TorqueVector tv = {.left = 1.0, .right = 1.0};
    if (torque_vector_enable) {
        tv = AdjustTorqueVectoring(sensors::driver::GetSteeringWheel());
    }

    float motor_torque_request = torque_request.Update(
        driver_torque_request, sensors::driver::GetBrakePercent());

    float torque = LUT::Evaluate(tuning::pedal_to_torque,
                                 motor_torque_request * tc_scale_factor);
    torque_ma.LoadValue(torque);
    float smoothed_torque = torque_ma.GetValue();

    left_request.torque_limit_positive = smoothed_torque * tv.left;
    right_request.torque_limit_positive = smoothed_torque * tv.right;
    // negative limit fields fixed at 0 in simulink model
}

}  // namespace vehicle_dynamics

/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "vehicle_dynamics.hpp"

#include "bindings.hpp"
#include "generated/can/veh_messages.hpp"
#include "lookup_table.hpp"
#include "motors/motors.hpp"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "thresholds.hpp"
#include "tuning.hpp"
#include "vehicle_dynamics_calc.hpp"

using namespace ctrl;
using namespace generated::can;
using Profile_t = RxDashStatus::Profile_t;
namespace vehicle_dynamics {

static float driver_torque_request;

static motors::Request left_request;
static motors::Request right_request;

static Profile_t profile;
static float target_slip_ratio;
static TractionControl traction_ctrl;
static bool torque_vector_enable;

amk::Request GetLeftMotorRequest(void) {
    return left_request;
}

amk::Request GetRightMotorRequest(void) {
    return right_request;
}

void SetProfile(generated::can::RxDashStatus::Profile_t _profile) {
    profile = _profile;
}

void SetTorqueVectorEnable(bool enable) {
    torque_vector_enable = enable;
}

void SetTargetSlipRatio(float target_slip) {
    target_slip_ratio = target_slip;
}

void SetDriverTorqueRequest(float _driver_torque_request) {
    driver_torque_request = _driver_torque_request;
}

void Init(void) {
    SetTorqueVectorEnable(false);
    SetTargetSlipRatio(0.2f);
    SetProfile(Profile_t::Default);

    // Fix speed request and control the torque limits
    left_request = {
        .speed_request = 0.f,
        .torque_limit_positive = 0.f,
        .torque_limit_negative = 0.f,
    };
    right_request = {
        .speed_request = 0.f,
        .torque_limit_positive = 0.f,
        .torque_limit_negative = 0.f,
    };

    driver_torque_request = 0;
    traction_ctrl.Init(bindings::GetTickMs());
}

void Update_100Hz(void) {
    // int time_ms = bindings::GetTickMs();

    // float actual_slip =
    //     CalculateActualSlip(sensors::dynamics::GetWheelSpeeds());
    //
    // float tc_scale_factor = traction_ctrl.UpdateScaleFactor(
    //     actual_slip, target_slip_ratio, time_ms);
    //
    float tc_scale_factor = 1.f;

    TorqueVector tv = {.left = 1.0, .right = 1.0};
    if (torque_vector_enable) {
        tv = AdjustTorqueVectoring(sensors::driver::GetSteeringWheel());
    }

    float torque = macfe::LookupTable::Evaluate(tuning::pedal_to_torque,
                                                driver_torque_request);

    left_request = {
        .speed_request = -threshold::MOTOR_SPEED_LIMIT_RPM,
        .torque_limit_positive = 0.f,
        .torque_limit_negative = -torque * tv.left * tc_scale_factor,
    };
    right_request = {
        .speed_request = threshold::MOTOR_SPEED_LIMIT_RPM,
        .torque_limit_positive = torque * tv.right * tc_scale_factor,
        .torque_limit_negative = 0.f,
    };
}

}  // namespace vehicle_dynamics

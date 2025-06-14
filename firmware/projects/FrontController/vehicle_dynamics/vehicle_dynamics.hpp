/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#pragma once

#include "generated/can/veh_messages.hpp"
#include "motors/motors.hpp"

namespace vehicle_dynamics {

motors::Request GetLeftMotorRequest(void);
motors::Request GetRightMotorRequest(void);

void SetTorqueVectorEnable(bool enable);
void SetTargetSlipRatio(float target_slip);
void SetProfile(generated::can::RxDashStatus::Profile_t profile);
void SetDriverTorqueRequest(float driver_torque_request);

void Init(void);
void Update_100Hz();

}  // namespace vehicle_dynamics

/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#pragma once

#include "generated/can/veh_messages.hpp"
#include "motors/motors.hpp"
#include "sensors/driver/driver.hpp"
#include "vehicle_dynamics_calc.hpp"

namespace vehicle_dynamics {

class VehicleDynamics {
public:
    explicit VehicleDynamics(sensors::driver::DriverSensors& sensors)
        : driver_sensors(sensors) {
        Init(0);
    }

    motors::Request GetLeftMotorRequest(void);
    motors::Request GetRightMotorRequest(void);

    void SetTorqueVectorEnable(bool enable);
    void SetTargetSlipRatio(float target_slip);
    void SetProfile(generated::can::RxDashStatus::Profile_t profile);
    void SetDriverTorqueRequest(float driver_torque_request);

    void Init(int time_ms);
    void Update_100Hz();

private:
    sensors::driver::DriverSensors& driver_sensors;
    float driver_torque_request = 0;
    motors::Request left_request{};
    motors::Request right_request{};
    generated::can::RxDashStatus::Profile_t profile{};
    float target_slip_ratio = 0;
    ctrl::TractionControl traction_ctrl{};
    bool torque_vector_enable = false;
};

}  // namespace vehicle_dynamics

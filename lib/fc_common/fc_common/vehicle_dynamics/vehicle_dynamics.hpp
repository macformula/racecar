/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#pragma once

#include "fc_common/motors/motors.hpp"
#include "fc_common/sensors/driver/driver.hpp"
#include "generated/can/veh_messages.hpp"
#include "vehicle_dynamics_calc.hpp"

namespace macfe::fc {

class VehicleDynamics {
public:
    explicit VehicleDynamics(macfe::fc::sensors::DriverSensors& sensors)
        : driver_sensors(sensors) {
        Init(0);
    }

    amk::Request GetLeftMotorRequest(void);
    amk::Request GetRightMotorRequest(void);

    void SetTorqueVectorEnable(bool enable);
    void SetTargetSlipRatio(float target_slip);
    void SetProfile(generated::can::RxDashStatus::Profile_t profile);
    void SetDriverTorqueRequest(float driver_torque_request);

    void Init(int time_ms);
    void Update_100Hz();

private:
    macfe::fc::sensors::DriverSensors& driver_sensors;
    float driver_torque_request = 0;
    amk::Request left_request{};
    amk::Request right_request{};
    generated::can::RxDashStatus::Profile_t profile{};
    float target_slip_ratio = 0;
    ctrl::TractionControl traction_ctrl{};
    bool torque_vector_enable = false;
};

}  // namespace macfe::fc

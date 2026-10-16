#pragma once

#include "fc-common/sensors/driver/driver.hpp"

namespace driver_interface {

class DriverInterface {
public:
    explicit DriverInterface(sensors::driver::DriverSensors& sensors)
        : driver_sensors(sensors) {}

    float GetTorqueRequest(void);
    bool IsBrakePressed(void);

    void Update_100Hz(void);

private:
    sensors::driver::DriverSensors& driver_sensors;
    bool pedal_needs_reset = false;  // rule EV.4.7
    float torque_request = 0.f;
};

}  // namespace driver_interface

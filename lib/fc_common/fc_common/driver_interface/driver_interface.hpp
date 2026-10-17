#pragma once

#include "fc_common/sensors/driver/driver.hpp"

namespace macfe::fc {

class DriverInterface {
public:
    explicit DriverInterface(macfe::fc::sensors::DriverSensors& sensors)
        : driver_sensors(sensors) {}

    float GetTorqueRequest(void);
    bool IsBrakePressed(void);

    void Update_100Hz(void);

private:
    macfe::fc::sensors::DriverSensors& driver_sensors;
    bool pedal_needs_reset = false;  // rule EV.4.7
    float torque_request = 0.f;
};

}  // namespace macfe::fc

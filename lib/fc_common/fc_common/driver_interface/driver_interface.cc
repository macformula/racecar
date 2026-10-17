#include "driver_interface.hpp"

#include "fc_common/alerts/alerts.hpp"
#include "fc_common/sensors/driver/driver.hpp"
#include "thresholds.hpp"

namespace macfe::fc {

void DriverInterface::Update_100Hz(void) {
    float apps_main = driver_sensors.GetAccelPercent1();
    float apps_backup = driver_sensors.GetAccelPercent2();

    bool apps_implausible = std::abs(apps_main - apps_backup) >
                            threshold::PEDAL_IMPLAUSIBLE_PERCENT;

    if (pedal_needs_reset) {
        if (apps_main < threshold::EV_4_7_APPS_DEACTIVATE) {
            pedal_needs_reset = false;
        }
    } else {
        if ((apps_main > threshold::EV_4_7_APPS_ACTIVATE) && IsBrakePressed()) {
            pedal_needs_reset = true;
        }
    }

    alerts::Get().apps_implausible = apps_implausible;
    alerts::Get().ev47_active = pedal_needs_reset;

    if (apps_implausible) {
        torque_request = 0.f;
    } else if (pedal_needs_reset) {
        torque_request = 0.f;
    } else {
        torque_request = apps_main;
    }
}

float DriverInterface::GetTorqueRequest(void) {
    return torque_request;
}

bool DriverInterface::IsBrakePressed(void) {
    return driver_sensors.GetBrakePercent() > threshold::BRAKES_PRESSED_PERCENT;
}

}  // namespace macfe::fc

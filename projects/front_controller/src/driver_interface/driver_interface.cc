#include "driver_interface.hpp"

#include "alerts/alerts.hpp"
#include "sensors/driver/driver.hpp"
#include "thresholds.hpp"

namespace driver_interface {

static bool pedal_needs_reset = false;  // rule EV.4.7

static float torque_request = 0.f;

void Update_100Hz(void) {
    float apps_main = sensors::driver::GetAccelPercent1();
    float apps_backup = sensors::driver::GetAccelPercent2();

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

    alerts::GetAlertsManager().SetPresent(alerts::FcAlert::AppsImplausible,
                                          apps_implausible);
    alerts::GetAlertsManager().SetPresent(alerts::FcAlert::EV47Active,
                                          pedal_needs_reset);

    if (apps_implausible) {
        torque_request = 0.f;
    } else if (pedal_needs_reset) {
        torque_request = 0.f;
    } else {
        torque_request = apps_main;
    }
}

float GetTorqueRequest(void) {
    return torque_request;
}

bool IsBrakePressed(void) {
    return sensors::driver::GetBrakePercent() >
           threshold::BRAKES_PRESSED_PERCENT;
}

}  // namespace driver_interface

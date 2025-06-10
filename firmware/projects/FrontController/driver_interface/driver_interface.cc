#include "driver_interface.hpp"

#include "alerts/alerts.hpp"
#include "sensors/driver/driver.hpp"
#include "thresholds.hpp"

namespace driver_interface {

float GetTorqueRequest(void) {
    double apps_diff = std::abs(sensors::driver::GetAccelPercent1() -
                                sensors::driver::GetAccelPercent2());

    if (apps_diff > threshold::PEDAL_IMPLAUSIBLE_PERCENT) {
        alerts::Get().apps_implausible = true;
        return 0;
    } else {
        alerts::Get().apps_implausible = false;
        return sensors::driver::GetAccelPercent1();
    }
}

bool IsBrakePressed(void) {
    return sensors::driver::GetBrakePercent() >
           threshold::BRAKES_PRESSED_PERCENT;
}

}  // namespace driver_interface

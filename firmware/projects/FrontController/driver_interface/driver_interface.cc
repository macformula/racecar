#include "driver_interface.hpp"

#include "governor/governor.hpp"
#include "sensors/driver/driver.hpp"
#include "thresholds.hpp"

namespace driver_interface {

float GetTorqueRequest(void) {
    if (governor::GetState() != governor::State::RUNNING) {
        return 0;
    }

    double apps_diff = std::abs(sensors::driver::GetAccelPercent1() -
                                sensors::driver::GetAccelPercent2());

    if (apps_diff > threshold::PEDAL_IMPLAUSIBLE_PERCENT) {
        return 0;
    }

    return sensors::driver::GetAccelPercent1();
}

bool IsBrakePressed(void) {
    return sensors::driver::GetBrakePercent() >
           threshold::BRAKES_PRESSED_PERCENT;
}

}  // namespace driver_interface

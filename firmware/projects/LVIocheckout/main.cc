/// @author Samuel Parent
/// @date 2024-04-27

#include <cstdint>
#include <regex>

#include "bindings.h"
#include "inc/bindings.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/identity.h"
#include "shared/util/mappers/mapper.h"

int main(void) {
    bindings::Initialize();

    while (true) {
        // bool dcdc_valid = bindings::dcdc_valid.Read();
        // TODO: set a digital out to this value.
        bindings::accumulator_en.SetHigh();
        bindings::dcdc_en.SetHigh();
        bindings::tsal_en.SetHigh();
        bindings::raspberry_pi_en.SetHigh();
        bindings::front_controller_en.SetHigh();
        bindings::speedgoat_en.SetHigh();
        bindings::motor_ctrl_precharge_en.SetHigh();
        bindings::motor_ctrl_en.SetHigh();
        bindings::imu_gps_en.SetHigh();
        bindings::shutdown_circuit_en.SetHigh();
        bindings::inverter_en.SetHigh();

        bindings::DelayMS(1000);

        bindings::accumulator_en.SetLow();
        bindings::dcdc_en.SetLow();
        bindings::tsal_en.SetLow();
        bindings::raspberry_pi_en.SetLow();
        bindings::front_controller_en.SetLow();
        bindings::speedgoat_en.SetLow();
        bindings::motor_ctrl_precharge_en.SetLow();
        bindings::motor_ctrl_en.SetLow();
        bindings::imu_gps_en.SetLow();
        bindings::shutdown_circuit_en.SetLow();
        bindings::inverter_en.SetLow();

        bindings::DelayMS(1000);
    }

    return 0;
}
/// @author Samuel Parent
/// @date 2024-04-27

#include <cstdint>
#include <regex>

#include "app.hpp"
#include "bindings.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/identity.hpp"
#include "shared/util/mappers/mapper.hpp"

Subsystem front_controller{bindings::front_controller_en};

int main(void) {
    bindings::Initialize();

    while (true) {
        // bool dcdc_valid = bindings::dcdc_valid.Read();
        // TODO: set a digital out to this value.
        // bindings::accumulator_en.SetHigh();
        // bindings::dcdc_en.SetHigh();
        // bindings::tsal_en.SetHigh();
        // bindings::raspberry_pi_en.SetHigh();
        // bindings::front_controller_en.SetHigh();

        for (int i = 0; i < 4; i++) {
            front_controller.Enable();
            bindings::DelayMS(500);
            front_controller.Disable();
            bindings::DelayMS(500);
        }

        // bindings::DelayMS(1000);

        // for (int i = 0; i < 2; i++) {
        //     bindings::front_controller_en.SetHigh();
        //     bindings::DelayMS(1000);
        //     bindings::front_controller_en.SetLow();
        //     bindings::DelayMS(1000);
        // }
        // bindings::DelayMS(1000);

        // bindings::speedgoat_en.SetHigh();
        // bindings::motor_ctrl_precharge_en.SetHigh();
        // bindings::motor_ctrl_en.SetHigh();
        // bindings::imu_gps_en.SetHigh();
        // bindings::shutdown_circuit_en.SetHigh();
        // bindings::inverter_en.SetHigh();

        // bindings::accumulator_en.SetLow();
        // bindings::dcdc_en.SetLow();
        // bindings::tsal_en.SetLow();
        // bindings::raspberry_pi_en.SetLow();

        // bindings::speedgoat_en.SetLow();
        // bindings::motor_ctrl_precharge_en.SetLow();
        // bindings::motor_ctrl_en.SetLow();
        // bindings::imu_gps_en.SetLow();
        // bindings::shutdown_circuit_en.SetLow();
        // bindings::inverter_en.SetLow();

        // bindings::DelayMS(1000);
    }

    return 0;
}
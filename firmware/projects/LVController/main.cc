/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <regex>

#include "app.h"
#include "bindings.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/identity.h"
#include "shared/util/mappers/mapper.h"

Subsystem tsal{bindings::tsal_en};
Subsystem raspberry_pi{bindings::raspberry_pi_en};
Subsystem front_controller{bindings::front_controller_en};
Subsystem speedgoat{bindings::speedgoat_en};
Subsystem accumulator{bindings::accumulator_en};
Subsystem motor_ctrl_precharge{bindings::motor_ctrl_precharge_en};
Subsystem motor_ctrl{bindings::motor_ctrl_en};
Subsystem imu_gps{bindings::imu_gps_en};
Subsystem shutdown_circuit{bindings::shutdown_circuit_en};

DCDC dcdc{
    bindings::dcdc_en,
    bindings::dcdc_valid,
    bindings::dcdc_led_en,
};
Subsystem powertrain_pump{
    bindings::powertrain_pump_en,
};

auto powertrain_fan_power_to_duty = shared::util::IdentityMap<float>();
Fan powertrain_fan{
    bindings::powertrain_fan_en,
    bindings::powertrain_fan_pwm,
    powertrain_fan_power_to_duty,
};

Subsystem all_subsystems[] = {
    tsal,        raspberry_pi,         front_controller, speedgoat,
    accumulator, motor_ctrl_precharge, motor_ctrl,       imu_gps,
    dcdc,        powertrain_pump,      powertrain_fan,
};

void DoPowerupSequence() {
    tsal.Enable();
    bindings::DelayMS(50);
    raspberry_pi.Enable();
    bindings::DelayMS(50);
    front_controller.Enable();
    bindings::DelayMS(100);
    speedgoat.Enable();
    bindings::DelayMS(100);
    motor_ctrl_precharge.Enable();
    bindings::DelayMS(2000);
    motor_ctrl.Enable();
    bindings::DelayMS(50);
    motor_ctrl_precharge.Disable();
    bindings::DelayMS(50);
    imu_gps.Enable();
}

void DoPowertrainEnableSequence() {
    dcdc.Enable();

    while (!dcdc.CheckValid()) continue;
    bindings::DelayMS(50);
    powertrain_pump.Enable();
    bindings::DelayMS(100);
    powertrain_fan.Enable();
    bindings::DelayMS(50);
    powertrain_fan.Dangerous_SetPowerNow(30);
    powertrain_fan.SetTargetPower(100, 14);

    float _update_period_sec = 0.1f;
    while (!powertrain_fan.IsAtTarget()) {
        bindings::DelayMS(uint32_t(_update_period_sec * 1000));
        powertrain_fan.Update(_update_period_sec);
    }
}

void DoPowertrainDisableSequence() {
    powertrain_pump.Disable();
    powertrain_fan.Disable();
}

int main(void) {
    bindings::Initialize();

    // Ensure all subsystems are disabled to start.
    for (auto sys : all_subsystems) {
        sys.Disable();
    }

    // Powerup sequence
    DoPowerupSequence();

    bool waiting_for_bms = true;
    while (waiting_for_bms) continue;
    shutdown_circuit.Enable();

    while (true) {
        bool waiting_for_vehicle_can = true;
        while (waiting_for_vehicle_can) continue;

        DoPowertrainEnableSequence();

        while (dcdc.CheckValid()) continue;

        DoPowertrainDisableSequence();
    }

    return 0;
}
/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <regex>

#include "app.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/mapper.h"

namespace bindings {

extern shared::periph::DigitalOutput& tsal_en;
extern shared::periph::DigitalOutput& raspberry_pi_en;
extern shared::periph::DigitalOutput& front_controller_en;
extern shared::periph::DigitalOutput& speedgoat_en;
extern shared::periph::DigitalOutput& accumulator_en;
extern shared::periph::DigitalOutput& motor_ctrl_precharge_en;
extern shared::periph::DigitalOutput& motor_ctrl_en;
extern shared::periph::DigitalOutput& imu_gps_en;

extern shared::periph::DigitalOutput& dcdc_en;
extern shared::periph::DigitalInput& dcdc_valid;
extern shared::periph::DigitalOutput& dcdc_led_en;
extern shared::periph::DigitalOutput& powertrain_pump_en;
extern shared::periph::DigitalOutput& powertrain_fan_en;
extern shared::periph::PWMOutput& powertrain_fan_pwm;
extern shared::util::Mapper<float>& powertrain_fan_power_to_duty;

extern Status dcdc_status;

extern void Initialize();
}  // namespace bindings

extern void DelayMS(uint32_t ms);

Subsystem tsal{bindings::tsal_en, false};
Subsystem raspberry_pi{bindings::raspberry_pi_en, false};
Subsystem front_controller{bindings::front_controller_en, false};
Subsystem speedgoat{bindings::speedgoat_en, false};
Subsystem accumulator{bindings::accumulator_en, false};
Subsystem motor_ctrl_precharge{bindings::motor_ctrl_precharge_en, false};
Subsystem motor_ctrl{bindings::motor_ctrl_en, false};
Subsystem imu_gps{bindings::imu_gps_en, false};

Subsystem dcdc{bindings::dcdc_en, false};
Status dcdc_status{bindings::dcdc_valid, true};
Subsystem powertrain_pump{bindings::powertrain_pump_en, false};
Fan powertrain_fan{
    bindings::powertrain_fan_en,
    false,
    bindings::powertrain_fan_pwm,
    bindings::powertrain_fan_power_to_duty,
};

Subsystem all_subsystems[] = {
    tsal,       raspberry_pi, front_controller,
    speedgoat,  accumulator,  motor_ctrl_precharge,
    motor_ctrl, imu_gps,      powertrain_fan,
};

void DoPowerupSequence() {
    tsal.Enable();
    DelayMS(50);
    raspberry_pi.Enable();
    DelayMS(50);
    front_controller.Enable();
    DelayMS(100);
    speedgoat.Enable();
    DelayMS(100);
    motor_ctrl_precharge.Enable();
    DelayMS(2000);
    motor_ctrl.Enable();
    DelayMS(50);
    motor_ctrl_precharge.Disable();
    DelayMS(50);
    imu_gps.Enable();
}

void DoPowertrainEnableSequence() {
    dcdc.Enable();
}

void DoPowertraiDisableSequence() {}

int main(void) {
    bindings::Initialize();

    // Ensure all subsystems are disabled to start.
    for (auto sys : all_subsystems) {
        sys.Disable();
    }

    // Powerup sequence
    DoPowerupSequence();

    while (true) {
    }

    return 0;
}
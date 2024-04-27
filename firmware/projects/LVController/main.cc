/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <regex>

#include "app.h"
#include "bindings.h"
#include "generated/can/can_messages.h"
#include "generated/can/msg_registry.h"
#include "shared/os/os.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/identity.h"
#include "shared/util/mappers/mapper.h"

namespace os {
extern void Tick(uint32_t ticks);
extern void InitializeKernel();
extern void StartKernel();
}  // namespace os

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
    constexpr float kDutyInitial = 30.0f;
    constexpr float kDutyFinal = 100.0f;
    constexpr float kSweepPeriodSec = 5.0f;
    constexpr float kMaxDutyRate =
        (kDutyFinal - kDutyInitial) / kSweepPeriodSec;
    constexpr float kUpdatePeriodSec = 0.05f;

    dcdc.Enable();

    while (!dcdc.CheckValid()) continue;
    bindings::DelayMS(50);
    powertrain_pump.Enable();
    bindings::DelayMS(100);
    powertrain_fan.Enable();
    bindings::DelayMS(50);
    powertrain_fan.Dangerous_SetPowerNow(kDutyInitial);
    powertrain_fan.SetTargetPower(kDutyFinal, kMaxDutyRate);

    while (!powertrain_fan.IsAtTarget()) {
        bindings::DelayMS(uint32_t(kUpdatePeriodSec * 1000));
        powertrain_fan.Update(kUpdatePeriodSec);
    }
}

void DoPowertrainDisableSequence() {
    powertrain_pump.Disable();
    powertrain_fan.Disable();
}

void TaskCheckDCDC(void* arg) {
    while (true) {
        dcdc.CheckValid();
        os::Tick(10);
    }
}
void TaskMainLoop(void* arg) {
    while (true) {
        /// Start: Sam code
        // wait for"Read VEHICLE_CAN Bus for MC+ and MC- Closed and Precharge
        // Open"
        bool waiting_for_vehicle_can = true;
        while (waiting_for_vehicle_can) continue;
        /// End: Sam code

        DoPowertrainEnableSequence();

        while (dcdc.CheckValid()) continue;

        DoPowertrainDisableSequence();
    }
}

int main(void) {
    bindings::Initialize();
    os::InitializeKernel();

    // Ensure all subsystems are disabled to start.
    for (auto sys : all_subsystems) {
        sys.Disable();
    }

    // Powerup sequence
    DoPowerupSequence();

    /// Start: Sam code
    // Wait for "Read VEHICLE_CAN bus for BMS Close Contactor Command Low"
    bool waiting_for_bms = true;
    while (waiting_for_bms) continue;
    /// End: Sam code

    shutdown_circuit.Enable();

    os::StartKernel();

    while (true) continue;  // all logic is now handled in the RTOS tasks.

    return 0;
}
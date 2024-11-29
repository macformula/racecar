/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <regex>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/app.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/identity.hpp"
#include "shared/util/mappers/mapper.hpp"

using namespace generated::can;

VehBus veh_can{bindings::veh_can_base};

StateBroadcaster state_tx{veh_can};

Subsystem tsal{bindings::tsal_en};
Subsystem raspberry_pi{bindings::raspberry_pi_en};
Subsystem front_controller{bindings::front_controller_en};
Subsystem speedgoat{bindings::speedgoat_en};
Subsystem accumulator{bindings::accumulator_en};
Subsystem motor_ctrl_precharge{bindings::motor_ctrl_precharge_en};
Subsystem motor_ctrl{bindings::motor_ctrl_en};
Subsystem imu_gps{bindings::imu_gps_en};
Subsystem shutdown_circuit{bindings::shutdown_circuit_en};
Subsystem inverter{bindings::inverter_switch_en};

auto dcdc_en_inverted =
    shared::periph::InvertedDigitalOutput(bindings::dcdc_en);

DCDC dcdc{
    dcdc_en_inverted,
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

    state_tx.UpdateState(LvControllerState::TsalEnabled);

    bindings::DelayMS(50);

    // raspberry_pi.Enable();
    state_tx.UpdateState(LvControllerState::RaspiEnabled);

    bindings::DelayMS(50);

    front_controller.Enable();
    state_tx.UpdateState(LvControllerState::FrontControllerEnabled);

    bindings::DelayMS(100);

    // speedgoat.Enable();
    state_tx.UpdateState(LvControllerState::SpeedgoatEnabled);

    bindings::DelayMS(100);

    motor_ctrl_precharge.Enable();
    state_tx.UpdateState(LvControllerState::MotorControllerPrechargeEnabled);

    bindings::DelayMS(2000);

    motor_ctrl.Enable();
    state_tx.UpdateState(LvControllerState::MotorControllerEnabled);

    bindings::DelayMS(50);

    motor_ctrl_precharge.Disable();
    state_tx.UpdateState(LvControllerState::MotorControllerPrechargeDisabled);

    bindings::DelayMS(50);

    // imu_gps.Enable();
    state_tx.UpdateState(LvControllerState::ImuGpsEnabled);
}

void DoPowertrainEnableSequence() {
    constexpr float kDutyInitial = 30.0f;
    constexpr float kDutyFinal = 100.0f;
    constexpr float kSweepPeriodSec = 3.0f;
    constexpr float kMaxDutyRate =
        (kDutyFinal - kDutyInitial) / kSweepPeriodSec;
    constexpr float kUpdatePeriodSec = 0.01f;

    dcdc.Enable();
    state_tx.UpdateState(LvControllerState::DcdcEnabled);

    do {
        state_tx.UpdateState(LvControllerState::WaitingForDcdcValid);
        bindings::DelayMS(50);
    } while (!dcdc.CheckValid());

    bindings::DelayMS(50);

    powertrain_pump.Enable();
    state_tx.UpdateState(LvControllerState::DcdcLedEnabled);

    bindings::DelayMS(100);

    powertrain_fan.Enable();
    state_tx.UpdateState(LvControllerState::PowertrainFanEnabled);

    bindings::DelayMS(50);

    powertrain_fan.Dangerous_SetPowerNow(kDutyInitial);
    powertrain_fan.SetTargetPower(kDutyFinal, kMaxDutyRate);

    do {
        state_tx.UpdateState(LvControllerState::PowertrainFanSweeping);
        bindings::DelayMS(uint32_t(kUpdatePeriodSec * 1000));
        powertrain_fan.Update(kUpdatePeriodSec);
    } while (!powertrain_fan.IsAtTarget());
}

void DoPowertrainDisableSequence() {
    powertrain_pump.Disable();
    powertrain_fan.Disable();
}

bool IsContactorsOpen() {
    auto contactor_states = veh_can.GetRxContactorStates();

    if (contactor_states) {
        return (contactor_states->PackPositive() == 0 &&
                contactor_states->PackNegative() == 0 &&
                contactor_states->PackPrecharge() == 0);
    } else {
        return false;  // No data received yet
    }
}

bool IsContactorsClosed() {
    auto contactor_states = veh_can.GetRxContactorStates();

    if (contactor_states) {
        return (contactor_states->PackPositive() == 1 &&
                contactor_states->PackNegative() == 1 &&
                contactor_states->PackPrecharge() == 0);
    } else {
        return false;  // No data received yet
    }
}

void DoInverterSwitchCheck() {
    auto inverter_command = veh_can.GetRxInverterCommand();

    if (inverter_command && inverter_command->EnableInverter()) {
        inverter.Enable();
    } else {
        inverter.Disable();
    }
}

int main(void) {
    bindings::Initialize();

    state_tx.UpdateState(LvControllerState::Startup);

    // Ensure all subsystems are disabled to start.
    for (auto sys : all_subsystems) {
        sys.Disable();
    }

    // Powerup sequence
    DoPowerupSequence();

    while (true) continue;  // stop after fc enable debug

    do {
        state_tx.UpdateState(LvControllerState::WaitingForOpenContactors);
        bindings::DelayMS(50);
    } while (!IsContactorsOpen());

    shutdown_circuit.Enable();

    state_tx.UpdateState(LvControllerState::ShutdownCircuitEnabled);

    while (true) {
        do {
            state_tx.UpdateState(LvControllerState::WaitingForClosedContactors);
            bindings::DelayMS(50);
        } while (!IsContactorsClosed());

        DoPowertrainEnableSequence();

        do {
            state_tx.UpdateState(LvControllerState::SequenceComplete);
            DoInverterSwitchCheck();
        } while (dcdc.CheckValid());

        state_tx.UpdateState(LvControllerState::LostDcdcValid);

        inverter.Disable();
        DoPowertrainDisableSequence();
    }

    return 0;
}
/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/app.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/util/mappers/identity.hpp"

using namespace generated::can;

VehBus veh_can{bindings::veh_can_base};

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
Subsystem powertrain_pump{bindings::powertrain_pump_en};

shared::periph::InvertedDigitalOutput dcdc_en_inverted(bindings::dcdc_en);

DCDC dcdc{dcdc_en_inverted, bindings::dcdc_valid, bindings::dcdc_led_en};

auto powertrain_fan_power_to_duty = shared::util::IdentityMap<float>();
Fan powertrain_fan{
    bindings::powertrain_fan_en,
    bindings::powertrain_fan_pwm,
    powertrain_fan_power_to_duty,
};

Subsystem all_subsystems[] = {
    tsal,        raspberry_pi,         front_controller, speedgoat,
    accumulator, motor_ctrl_precharge, motor_ctrl,       imu_gps,
    dcdc,        powertrain_pump,      powertrain_fan,   shutdown_circuit};

void BroadcastState(LvControllerState state) {
    veh_can.Send(TxLvControllerStatus{static_cast<uint8_t>(state)});
}

void DoPowerupSequence() {
    tsal.Enable();
    BroadcastState(LvControllerState::TsalEnabled);

    bindings::DelayMS(50);

    raspberry_pi.Enable();
    BroadcastState(LvControllerState::RaspiEnabled);

    bindings::DelayMS(50);

    front_controller.Enable();
    BroadcastState(LvControllerState::FrontControllerEnabled);

    bindings::DelayMS(100);

    speedgoat.Enable();
    BroadcastState(LvControllerState::SpeedgoatEnabled);

    bindings::DelayMS(100);

    motor_ctrl_precharge.Enable();
    BroadcastState(LvControllerState::MotorControllerPrechargeEnabled);

    bindings::DelayMS(2000);

    motor_ctrl.Enable();
    BroadcastState(LvControllerState::MotorControllerEnabled);

    bindings::DelayMS(50);

    motor_ctrl_precharge.Disable();
    BroadcastState(LvControllerState::MotorControllerPrechargeDisabled);

    bindings::DelayMS(50);

    imu_gps.Enable();
    BroadcastState(LvControllerState::ImuGpsEnabled);
}

void SweepFanBlocking() {
    constexpr float kDutyInitial = 30.0f;
    constexpr float kDutyFinal = 100.0f;
    constexpr float kSweepPeriodSec = 3.0f;
    constexpr float kMaxDutyRate =
        (kDutyFinal - kDutyInitial) / kSweepPeriodSec;
    constexpr uint32_t kUpdatePeriodMS = 10;

    powertrain_fan.Dangerous_SetPowerNow(kDutyInitial);
    powertrain_fan.SetTargetPower(kDutyFinal, kMaxDutyRate);

    while (!powertrain_fan.IsAtTarget()) {
        bindings::DelayMS(kUpdatePeriodMS);
        powertrain_fan.Update(kUpdatePeriodMS / 1000.0f);
    }
}

void DoPowertrainEnableSequence() {
    dcdc.Enable();
    BroadcastState(LvControllerState::DcdcEnabled);

    BroadcastState(LvControllerState::WaitingForDcdcValid);
    while (!dcdc.CheckValid()) bindings::DelayMS(50);

    bindings::DelayMS(50);

    powertrain_pump.Enable();
    BroadcastState(LvControllerState::PowertrainPumpEnabled);

    bindings::DelayMS(100);

    powertrain_fan.Enable();
    BroadcastState(LvControllerState::PowertrainFanEnabled);

    bindings::DelayMS(50);

    BroadcastState(LvControllerState::PowertrainFanSweeping);
    SweepFanBlocking();  // is it possible that this never returns?
}

bool IsContactorsOpen() {
    auto contactor_states = veh_can.GetRxContactorStates();

    if (contactor_states.has_value()) {
        return (contactor_states->PackPositive() == 0 &&
                contactor_states->PackNegative() == 0 &&
                contactor_states->PackPrecharge() == 0);
    } else {
        return false;
    }
}

bool IsContactorsClosed() {
    auto contactor_states = veh_can.GetRxContactorStates();

    if (contactor_states.has_value()) {
        return (contactor_states->PackPositive() == 1 &&
                contactor_states->PackNegative() == 1 &&
                contactor_states->PackPrecharge() == 0);
    } else {
        return false;
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

    BroadcastState(LvControllerState::Startup);

    // Ensure all subsystems are disabled to start.
    for (auto& sys : all_subsystems) {
        sys.Disable();
    }

    DoPowerupSequence();

    BroadcastState(LvControllerState::WaitingForOpenContactors);
    while (!IsContactorsOpen()) {
        bindings::DelayMS(50);
    }

    shutdown_circuit.Enable();

    BroadcastState(LvControllerState::ShutdownCircuitEnabled);

    while (true) {
        BroadcastState(LvControllerState::WaitingForClosedContactors);
        while (!IsContactorsClosed()) {
            bindings::DelayMS(50);
        }

        DoPowertrainEnableSequence();
        BroadcastState(LvControllerState::SequenceComplete);

        while (dcdc.CheckValid()) {
            DoInverterSwitchCheck();
            bindings::DelayMS(50);
        }

        BroadcastState(LvControllerState::LostDcdcValid);

        inverter.Disable();
        powertrain_pump.Disable();
        powertrain_fan.Disable();
    }

    return 0;
}
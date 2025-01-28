/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <optional>

#include "app/dcdc.hpp"
#include "app/fan.hpp"
#include "app/subsystem.hpp"
#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
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
Subsystem motor_ctrl_switch{bindings::motor_ctrl_switch_en};
Subsystem powertrain_pump{bindings::powertrain_pump_en};

shared::periph::InvertedDigitalOutput dcdc_en_inverted(bindings::dcdc_en);

DCDC dcdc{dcdc_en_inverted, bindings::dcdc_valid};

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

class StateMachine {
public:
    void Initialize(int time_ms) {
        state_enter_time_ = time_ms;
    }

    void Update(int time_ms) {
        using enum State;

        auto transition = Transition(time_ms - state_enter_time_);
        bool on_enter = transition.has_value();

        if (on_enter) {
            state_ = transition.value();
            state_enter_time_ = time_ms;
            // fix this line - need to update DBC enum
            veh_can.Send(TxLvControllerStatus{static_cast<uint8_t>(state_)});
        }

        switch (state_) {
            case PWRUP_START:
                break;

            case PWRUP_TSAL_ON:
                if (on_enter) tsal.Enable();
                break;

            case PWRUP_RASPI_ON:
                if (on_enter) raspberry_pi.Enable();
                break;

            case PWRUP_FRONT_CONTROLLER_ON:
                if (on_enter) front_controller.Enable();
                break;

            case PWRUP_SPEEDGOAT_ON:
                if (on_enter) speedgoat.Enable();
                break;

            case PWRUP_ACCUMULATOR_ON:
                if (on_enter) accumulator.Enable();
                break;

            case PWRUP_MOTOR_PRECHARGE_ON:
                if (on_enter) motor_ctrl_precharge.Enable();
                break;

            case PWRUP_MOTOR_LV:
                if (on_enter) motor_ctrl.Enable();
                break;

            case PWRUP_MOTOR_PRECHARGE_OFF:
                if (on_enter) motor_ctrl_precharge.Disable();
                break;

            case PWRUP_IMU_GPS_ON:
                if (on_enter) imu_gps.Enable();
                break;

            case PWRUP_SHUTDOWN_ON:
                if (on_enter) shutdown_circuit.Enable();
                break;

            case WAITING_FOR_DCDC:
                if (on_enter) dcdc.Enable();
                break;

            case POWERTRAIN_PUMP_ON:
                if (on_enter) powertrain_pump.Enable();
                break;

            case POWERTRAIN_FAN_SWEEP: {
                constexpr float kPowerInitial = 30.0f;
                constexpr float kPowerFinal = 100.0f;
                constexpr float kSweepPeriodMs = 3000.0f;

                if (on_enter) {
                    powertrain_fan.StartSweep({
                        static_cast<float>(time_ms),
                        kSweepPeriodMs,
                        kPowerInitial,
                        kPowerFinal,
                    });
                } else {
                    powertrain_fan.UpdateSweep(time_ms);
                }

            } break;

            case RUNNING:
                break;

            case SHUTDOWN_PUMP_OFF:
                if (on_enter) powertrain_pump.Disable();
                break;

            case SHUTDOWN_FAN_OFF:
                if (on_enter) powertrain_fan.Disable();
                break;
        }
    }

private:
    enum class State {
        PWRUP_START,
        PWRUP_TSAL_ON,
        PWRUP_RASPI_ON,
        PWRUP_FRONT_CONTROLLER_ON,
        PWRUP_SPEEDGOAT_ON,
        PWRUP_ACCUMULATOR_ON,
        PWRUP_MOTOR_PRECHARGE_ON,
        PWRUP_MOTOR_LV,
        PWRUP_MOTOR_PRECHARGE_OFF,
        PWRUP_IMU_GPS_ON,
        PWRUP_SHUTDOWN_ON,
        WAITING_FOR_DCDC,
        POWERTRAIN_PUMP_ON,
        POWERTRAIN_FAN_SWEEP,
        RUNNING,
        SHUTDOWN_PUMP_OFF,
        SHUTDOWN_FAN_OFF,
    };

    std::optional<State> Transition(int elapsed) {
        using enum State;

        if (state_ == WAITING_FOR_DCDC || state_ == POWERTRAIN_PUMP_ON ||
            state_ == POWERTRAIN_FAN_SWEEP || state_ == RUNNING) {
            if (!dcdc.CheckValid()) return SHUTDOWN_PUMP_OFF;
        }

        switch (state_) {
            case PWRUP_START:
                if (elapsed > 50) return PWRUP_TSAL_ON;
                break;

            case PWRUP_TSAL_ON:
                if (elapsed > 50) return PWRUP_RASPI_ON;
                break;

            case PWRUP_RASPI_ON:
                if (elapsed > 50) return PWRUP_FRONT_CONTROLLER_ON;
                break;

            case PWRUP_FRONT_CONTROLLER_ON:
                if (elapsed > 100) return PWRUP_SPEEDGOAT_ON;
                break;

            case PWRUP_SPEEDGOAT_ON:
                if (elapsed > 200) return PWRUP_ACCUMULATOR_ON;
                break;

            case PWRUP_ACCUMULATOR_ON:
                if (elapsed > 100) return PWRUP_MOTOR_PRECHARGE_ON;
                break;

            case PWRUP_MOTOR_PRECHARGE_ON:
                if (elapsed > 2000) return PWRUP_MOTOR_LV;
                break;

            case PWRUP_MOTOR_LV:
                if (elapsed > 50) return PWRUP_MOTOR_PRECHARGE_OFF;
                break;

            case PWRUP_MOTOR_PRECHARGE_OFF:
                if (elapsed > 50) return PWRUP_IMU_GPS_ON;
                break;

            case PWRUP_IMU_GPS_ON: {
                auto contactors = veh_can.GetRxContactorStates();
                if (contactors.has_value()) {
                    if (contactors->PackPositive() == false &&
                        contactors->PackNegative() == false &&
                        contactors->PackPrecharge() == false) {
                        return PWRUP_SHUTDOWN_ON;
                    }
                }
            } break;

            case PWRUP_SHUTDOWN_ON: {
                auto contactors = veh_can.GetRxContactorStates();
                if (contactors.has_value()) {
                    if (contactors->PackPositive() == true &&
                        contactors->PackNegative() == true &&
                        contactors->PackPrecharge() == false) {
                        return WAITING_FOR_DCDC;
                    }
                }
            } break;

            case WAITING_FOR_DCDC:
                if (dcdc.CheckValid() && elapsed > 50) {
                    return POWERTRAIN_PUMP_ON;
                }
                break;

            case POWERTRAIN_PUMP_ON:
                if (elapsed > 100) return POWERTRAIN_FAN_SWEEP;
                break;

            case POWERTRAIN_FAN_SWEEP:
                if (elapsed > 10000 || powertrain_fan.IsSweepComplete()) {
                    return RUNNING;
                }
                break;

            case RUNNING:
                // DCDC invalid transition is handled before the switch-case
                break;

            case SHUTDOWN_PUMP_OFF:
                if (elapsed > 50) return SHUTDOWN_FAN_OFF;
                break;

            case SHUTDOWN_FAN_OFF:
                if (elapsed > 50) return PWRUP_SHUTDOWN_ON;
                break;
        }
        return std::nullopt;  // no transition -> stay in current state
    }

    State state_;
    int state_enter_time_;
};

int main(void) {
    bindings::Initialize();

    // Ensure all subsystems are disabled to start.
    for (auto& sys : all_subsystems) {
        sys.Disable();
    }

    StateMachine fsm;
    fsm.Initialize(bindings::GetTick());

    const int kUpdatePeriodMs = 100;
    while (true) {
        fsm.Update(bindings::GetTick());

        auto inverter_command = veh_can.GetRxInverterCommand();
        if (inverter_command.has_value() &&
            inverter_command->EnableInverter()) {
            motor_ctrl_switch.Enable();
        } else {
            motor_ctrl_switch.Disable();
        }

        bindings::DelayMS(kUpdatePeriodMs);
    }

    return 0;
}
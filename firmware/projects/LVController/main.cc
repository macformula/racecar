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

Subsystem tssi{bindings::tssi_en};
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
    tssi,        raspberry_pi,         front_controller, speedgoat,
    accumulator, motor_ctrl_precharge, motor_ctrl,       imu_gps,
    dcdc,        powertrain_pump,      powertrain_fan,   shutdown_circuit};

class StateMachine {
    // The timing in this state machine is not perfect. Event transitions are
    // processed AFTER outputs are set, so the new state output will not be
    // realized until the next Update().
    //
    //                                            state2
    // OUTPUT     state1          state1        (reaction)
    //           ---|-----:---------|---------------|---------
    // TRANSITION       event   ->state2
    //
    // If the transitions were handled BEFORE the outputs, then the reaction
    // would happen sooner. This improves reaction time by 1 Update() period on
    // average.
    //
    // TRANSITION       event   ->state2
    //           ---|-----:---------|---------------|---------
    // OUTPUT     state1          state2          state2
    //                          (reaction)
    //
    // However, the latter requires 2 separate switch-cases inside Update(): one
    // to handle events and the other to process the output.
    //
    // None of the events in LVController are that time sensitive, so the much
    // shorter and clearer code of the first implementation is justified.

public:
    StateMachine() : state_(State::PWRUP_START), on_enter_(true) {}

    void Update(int time_ms) {
        using enum State;

        std::optional<State> transition = std::nullopt;
        int elapsed = time_ms - state_enter_time_;

        // fix this line - need to update DBC enum
        veh_can.Send(TxLvControllerStatus{static_cast<uint8_t>(state_)});

        switch (state_) {
            case PWRUP_START:
                if (elapsed > 50) transition = PWRUP_TSSI_ON;
                break;

            case PWRUP_TSSI_ON:
                if (on_enter_) tssi.Enable();

                if (elapsed > 50) transition = PWRUP_RASPI_ON;
                break;

            case PWRUP_RASPI_ON:
                if (on_enter_) raspberry_pi.Enable();

                if (elapsed > 50) transition = PWRUP_FRONT_CONTROLLER_ON;
                break;

            case PWRUP_FRONT_CONTROLLER_ON:
                if (on_enter_) front_controller.Enable();

                if (elapsed > 100) transition = PWRUP_SPEEDGOAT_ON;
                break;

            case PWRUP_SPEEDGOAT_ON:
                if (on_enter_) speedgoat.Enable();

                if (elapsed > 200) transition = PWRUP_ACCUMULATOR_ON;
                break;

            case PWRUP_ACCUMULATOR_ON:
                if (on_enter_) accumulator.Enable();

                if (elapsed > 100) transition = PWRUP_MOTOR_PRECHARGE_ON;
                break;

            case PWRUP_MOTOR_PRECHARGE_ON:
                if (on_enter_) motor_ctrl_precharge.Enable();

                if (elapsed > 2000) transition = PWRUP_MOTOR_LV;
                break;

            case PWRUP_MOTOR_LV:
                if (on_enter_) motor_ctrl.Enable();

                if (elapsed > 50) transition = PWRUP_MOTOR_PRECHARGE_OFF;
                break;

            case PWRUP_MOTOR_PRECHARGE_OFF:
                if (on_enter_) motor_ctrl_precharge.Disable();

                if (elapsed > 50) transition = PWRUP_IMU_GPS_ON;
                break;

            case PWRUP_IMU_GPS_ON:
                if (on_enter_) imu_gps.Enable();

                {
                    auto contactors = veh_can.GetRxContactorStates();
                    if (contactors.has_value()) {
                        if (contactors->PackPositive() == false &&
                            contactors->PackNegative() == false &&
                            contactors->PackPrecharge() == false) {
                            transition = PWRUP_SHUTDOWN_ON;
                        }
                    }
                }
                break;

            case PWRUP_SHUTDOWN_ON:
                if (on_enter_) shutdown_circuit.Enable();

                {
                    auto contactors = veh_can.GetRxContactorStates();
                    if (contactors.has_value()) {
                        if (contactors->PackPositive() == true &&
                            contactors->PackNegative() == true &&
                            contactors->PackPrecharge() == false) {
                            transition = WAITING_FOR_DCDC;
                        }
                    }
                }
                break;

            case WAITING_FOR_DCDC:
                if (on_enter_) dcdc.Enable();

                if (dcdc.CheckValid() && elapsed > 50) {
                    transition = POWERTRAIN_PUMP_ON;
                }
                break;

            case POWERTRAIN_PUMP_ON:
                if (on_enter_) powertrain_pump.Enable();

                if (elapsed > 100) transition = POWERTRAIN_FAN_SWEEP;
                break;

            case POWERTRAIN_FAN_SWEEP: {
                constexpr float kPowerInitial = 30.0f;
                constexpr float kPowerFinal = 100.0f;
                constexpr float kSweepPeriodMs = 3000.0f;

                if (on_enter_) {
                    powertrain_fan.StartSweep({
                        static_cast<float>(time_ms),
                        kSweepPeriodMs,
                        kPowerInitial,
                        kPowerFinal,
                    });
                } else {
                    powertrain_fan.UpdateSweep(time_ms);
                }
            }
                if (elapsed > 10000 || powertrain_fan.IsSweepComplete()) {
                    transition = RUNNING;
                }

                break;

            case RUNNING:
                // DCDC invalid transition is handled after the switch-case
                break;

            case SHUTDOWN_PUMP_OFF:
                if (on_enter_) powertrain_pump.Disable();

                if (elapsed > 50) transition = SHUTDOWN_FAN_OFF;
                break;

            case SHUTDOWN_FAN_OFF:
                if (on_enter_) powertrain_fan.Disable();

                if (elapsed > 50) transition = PWRUP_SHUTDOWN_ON;
                break;
        }

        if (state_ == WAITING_FOR_DCDC || state_ == POWERTRAIN_PUMP_ON ||
            state_ == POWERTRAIN_FAN_SWEEP || state_ == RUNNING) {
            if (!dcdc.CheckValid()) {
                transition = SHUTDOWN_PUMP_OFF;
            }
        }

        on_enter_ = transition.has_value();
        if (on_enter_) {
            state_enter_time_ = time_ms;
            state_ = transition.value();
        }
    }

private:
    // State enum should be generated from CANGEN
    enum class State {
        PWRUP_START,
        PWRUP_TSSI_ON,
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

    State state_;
    int state_enter_time_;
    bool on_enter_;
};

TxSuspensionTravel MeasureSuspension() {}

int main(void) {
    bindings::Initialize();

    for (auto& sys : all_subsystems) {
        // This could be put into the PWRUP_START state handler
        sys.Disable();
    }

    StateMachine fsm;

    const int kUpdatePeriodMs = 200;
    while (true) {
        fsm.Update(bindings::GetTick());

        auto inverter_command = veh_can.GetRxInverterCommand();
        if (inverter_command.has_value() &&
            inverter_command->EnableInverter()) {
            motor_ctrl_switch.Enable();
        } else {
            motor_ctrl_switch.Disable();
        }

        TxSuspensionTravel suspension_msg{
            // todo: update the mapping
            .stp1 = static_cast<uint8_t>(bindings::suspension_travel1.Read()),
            .stp2 = static_cast<uint8_t>(bindings::suspension_travel2.Read()),
            .stp3 = static_cast<uint8_t>(bindings::suspension_travel3.Read()),
            .stp4 = static_cast<uint8_t>(bindings::suspension_travel4.Read()),
        };
        veh_can.Send(suspension_msg);

        bindings::DelayMS(kUpdatePeriodMs);
    }

    return 0;
}
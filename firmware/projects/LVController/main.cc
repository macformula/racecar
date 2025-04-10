/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <optional>

#include "app/dcdc.hpp"
#include "app/fan.hpp"
#include "app/tssi.hpp"
#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/util/mappers/identity.hpp"

using namespace generated::can;

VehBus veh_can{bindings::veh_can_base};

TSSI tssi{bindings::tssi_en, bindings::tssi_green_signal,
          bindings::tssi_red_signal};

DCDC dcdc{bindings::dcdc_en, bindings::dcdc_sense_select, bindings::dcdc_sense};

auto powertrain_fan_power_to_duty = shared::util::IdentityMap<float>();
Fans powertrain_fans{
    bindings::powertrain_fan1_en,
    bindings::powertrain_fan2_en,
    bindings::powertrain_fan_pwm,
    powertrain_fan_power_to_duty,
};

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
    using LvState = TxLvControllerStatus::LvState_t;

    StateMachine(int start_time)
        : state_(LvState::PWRUP_START), state_enter_time_(start_time) {}

    void Update(int time_ms) {
        using enum LvState;

        std::optional<LvState> transition = std::nullopt;
        int elapsed = time_ms - state_enter_time_;

        veh_can.Send(TxLvControllerStatus{
            .lv_state = state_, .elapsed = elapsed, .flag = flag});

        switch (state_) {
            case PWRUP_START:
                if (on_enter_) {
                    tssi.Disable();
                    dcdc.Disable();
                    powertrain_fans.Disable();

                    bindings::front_controller_en.SetLow();
                    bindings::accumulator_en.SetLow();
                    bindings::motor_ctrl_precharge_en.SetLow();
                    bindings::motor_controller_en.SetLow();
                    bindings::imu_gps_en.SetLow();
                    bindings::powertrain_pump1_en.SetLow();
                    bindings::powertrain_pump2_en.SetLow();
                    bindings::shutdown_circuit_en.SetLow();
                }
                if (elapsed > 50) transition = PWRUP_TSSI_ON;
                break;

            case PWRUP_TSSI_ON:
                if (on_enter_) tssi.Enable();

                if (elapsed > 50) transition = PWRUP_PERIPHERALS_ON;
                break;

            case PWRUP_PERIPHERALS_ON:
                if (on_enter_) {
                    bindings::front_controller_en.SetHigh();
                    bindings::imu_gps_en.SetHigh();
                }

                if (elapsed > 50) {
                    auto msg = veh_can.GetRxFC_Status();
                    if (msg.has_value()) {
                        // don't actually care about the status, just that FC
                        // has come online
                        transition = PWRUP_CHECK_HASH_STATUS;
                    }
                }
                break;

            case PWRUP_CHECK_HASH_STATUS: {
                veh_can.Send(
                    TxSyncHashVersion(generated::can::kVehDbcHashVersion));
                auto msg = veh_can.GetRxFC_Status();

                if (msg.has_value() &&
                    msg->HashStatus() ==
                        static_cast<uint8_t>(HashStatus::VALID)) {
                    transition = PWRUP_ACCUMULATOR_ON;
                }
            } break;

            case PWRUP_ACCUMULATOR_ON: {
                if (on_enter_) bindings::accumulator_en.SetHigh();

                auto contactors = veh_can.GetRxContactorCommand();
                if (contactors.has_value()) {
                    if (contactors->PackPositive() == false &&
                        contactors->PackNegative() == false &&
                        contactors->PackPrecharge() == false) {
                        transition = PWRUP_MOTOR_PRECHARGE_ON;
                    }
                }
            } break;

            case PWRUP_MOTOR_PRECHARGE_ON:
                if (on_enter_) bindings::motor_ctrl_precharge_en.SetHigh();

                if (elapsed > 2000) transition = PWRUP_MOTOR_LV;
                break;

            case PWRUP_MOTOR_LV:
                if (on_enter_) bindings::motor_controller_en.SetHigh();

                if (elapsed > 50) transition = PWRUP_MOTOR_PRECHARGE_OFF;
                break;

            case PWRUP_MOTOR_PRECHARGE_OFF:
                if (on_enter_) bindings::motor_ctrl_precharge_en.SetLow();

                if (elapsed > 50) transition = PWRUP_SHUTDOWN_ON;
                break;

            case PWRUP_SHUTDOWN_ON:
                if (on_enter_) bindings::shutdown_circuit_en.SetHigh();

                {
                    auto contactors =
                        veh_can.GetRxContactorCommand();  // fix to feedback
                    if (contactors.has_value()) {
                        if (contactors->PackPositive() == true &&
                            contactors->PackNegative() == true &&
                            contactors->PackPrecharge() == false) {
                            transition = MOTOR_CONTROLLER_SWITCH_SEQ;
                        }
                    }
                }
                break;

            case MOTOR_CONTROLLER_SWITCH_SEQ: {
                // static uint8_t switch_step = 0;
                // if (on_enter_) switch_step = 0;

                bindings::motor_ctrl_switch_en.SetLow();

                if (elapsed > 5000) {
                    auto m = veh_can.GetRxLvSwitch();
                    if (m.has_value()) {
                        bindings::motor_ctrl_switch_en.Set(m->SwitchClose());
                    }
                }

                // // verify this logic --> DEFINITELY INCORRECT
                // switch (switch_step) {
                //     case 0:
                //         break;
                //     case 1:
                //         bindings::motor_ctrl_switch_en.SetLow();
                //         break;
                //     case 2:
                //         bindings::motor_ctrl_switch_en.SetHigh();
                //         transition = DCDC_ON;
                //         break;
                // }
                // switch_step++;
            } break;

            case DCDC_ON:
                // should this be inverted?
                if (on_enter_) bindings::dcdc_en.SetHigh();

                if (elapsed > 100) transition = POWERTRAIN_PUMP_ON;
                break;

            case POWERTRAIN_PUMP_ON:
                if (on_enter_) {
                    bindings::powertrain_pump1_en.SetHigh();
                    bindings::powertrain_pump2_en.SetHigh();
                }

                if (elapsed > 100) transition = POWERTRAIN_FAN_ON;
                break;

            case POWERTRAIN_FAN_ON:
                if (on_enter_) powertrain_fans.Enable();

                if (elapsed > 50) transition = POWERTRAIN_FAN_SWEEP;
                break;

            case POWERTRAIN_FAN_SWEEP: {
                constexpr float kPowerInitial = 30.0f;
                constexpr float kPowerFinal = 100.0f;
                constexpr float kSweepPeriodMs = 3000.0f;

                if (on_enter_) {
                    powertrain_fans.StartSweep({
                        static_cast<float>(time_ms),
                        kSweepPeriodMs,
                        kPowerInitial,
                        kPowerFinal,
                    });
                } else {
                    powertrain_fans.UpdateSweep(time_ms);
                }
            }
                if (elapsed > 10000 || powertrain_fans.IsSweepComplete()) {
                    transition = READY_TO_DRIVE;
                }

                break;

            case READY_TO_DRIVE:
                break;

            case SHUTDOWN_DRIVER_WARNING:
                // This state's action is "sending the State over CAN" which
                // happens before the switch

                // Give the driver 30 s to stop before we make them.
                if (elapsed > 30000) transition = SHUTDOWN_PUMP_OFF;
                break;

            case SHUTDOWN_PUMP_OFF:
                if (on_enter_) {
                    bindings::powertrain_pump1_en.SetLow();
                    bindings::powertrain_pump2_en.SetLow();
                }

                if (elapsed > 50) transition = SHUTDOWN_FAN_OFF;
                break;

            case SHUTDOWN_FAN_OFF:
                if (on_enter_) powertrain_fans.Disable();

                if (elapsed > 50) transition = SHUTDOWN_COMPLETE;
                break;

            case SHUTDOWN_COMPLETE:
                // there's no coming back from this
                break;
        }

        // Shutdown checks that can occur from multiple states
        bool check_shutdown =
            state_ == DCDC_ON || state_ == POWERTRAIN_PUMP_ON ||
            state_ == POWERTRAIN_FAN_ON || state_ == POWERTRAIN_FAN_SWEEP ||
            state_ == READY_TO_DRIVE;

        if (false && check_shutdown) {  // temporary -> not on EV5
            const float kNoCurrentThresholdAmp = 0.5;  // what should this be?
            bool lost_lv_comms =
                false;  // this should come from a SPI heartbeat

            if (dcdc.MeasureVoltage() < 19.2 ||
                (lost_lv_comms &&
                 dcdc.MeasureAmps() < kNoCurrentThresholdAmp)) {
                transition = SHUTDOWN_DRIVER_WARNING;
            }

            auto msg = veh_can.GetRxFC_Status();
            // remove static_cast once cangen properly handles enums
            if (msg.has_value())
                // -1 is NOT SENT by FC as of 2025/02/12. This should be updated
                // in the future once FC sends useful statuses
                if (msg->GovStatus() == static_cast<uint8_t>(-1)) {
                    transition = SHUTDOWN_PUMP_OFF;
                }
        }

        // If a transition was indicated, handle it
        on_enter_ = transition.has_value();
        if (on_enter_) {
            state_enter_time_ = time_ms;
            state_ = transition.value();
        }
    }

private:
    LvState state_;  // enum defined by cangen
    int state_enter_time_;
    bool on_enter_;
    bool flag = false;  // temp
};

void UpdateBrakeLight() {
    auto msg = veh_can.GetRxBrakeLight();
    if (msg.has_value()) {
        bindings::brake_light_en.Set(msg->Enable());
    } else {
        bindings::brake_light_en.SetHigh();
    }
}

int main(void) {
    bindings::Initialize();

    StateMachine fsm(bindings::GetTick());

    const int kUpdatePeriodMs = 20;
    while (true) {
        int time_ms = bindings::GetTick();
        fsm.Update(time_ms);

        // {  // temporarily commented -> may be causing problems on EV5 TSAL
        //     tssi.Update(bindings::bms_fault.Read(),
        //     bindings::imd_fault.Read(),
        //                 time_ms);
        // }

        bindings::tssi_en.Set(time_ms & (1 << 11));

        UpdateBrakeLight();

        TxSuspensionTravel34 suspension_msg{
            // todo: update the mapping
            .stp3 = static_cast<uint8_t>(
                bindings::suspension_travel3.ReadVoltage()),
            .stp4 = static_cast<uint8_t>(
                bindings::suspension_travel4.ReadVoltage()),
        };
        veh_can.Send(suspension_msg);

        auto msg = veh_can.GetRxInitiateCanFlash();

        if (msg.has_value() &&
            msg->ECU() == RxInitiateCanFlash::ECU_t::LvController) {
            bindings::SoftwareReset();
        }

        bindings::DelayMS(kUpdatePeriodMs);
    }

    return 0;
}
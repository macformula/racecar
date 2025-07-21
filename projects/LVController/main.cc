/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <optional>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "shared/periph/gpio.hpp"

// LV Modules
#include "accumulator/accumulator.hpp"
#include "brakelight/brakelight.hpp"
#include "dcdc/dcdc.hpp"
#include "fans/fans.hpp"
#include "motor_controller/motor_controller.hpp"
#include "scheduler/scheduler.hpp"
#include "suspension/suspension.hpp"
#include "tssi/tssi.hpp"

using namespace generated::can;

VehBus veh_can{bindings::veh_can_base};

namespace fsm {

using LvState = TxLvStatus::LvState_t;

static LvState state = LvState::PWRUP_START;
static uint32_t elapsed = 0;

void Init(void) {
    state = LvState::PWRUP_START;
    elapsed = 0;
}

void Update_100hz(void) {
    using enum LvState;

    LvState new_state = state;

    switch (state) {
        case PWRUP_START:
            tssi::SetEnabled(false);
            dcdc::SetEnabled(false);
            fans::SetPowerSetpoint(0);
            motor_controller::SetEnabled(false);
            accumulator::SetEnabled(false);

            bindings::front_controller_en.SetLow();
            bindings::imu_gps_en.SetLow();
            bindings::powertrain_pump1_en.SetLow();
            bindings::powertrain_pump2_en.SetLow();
            bindings::shutdown_circuit_en.SetLow();

            if (elapsed > 50) {
                new_state = PWRUP_TSSI_ON;
            }
            break;

        case PWRUP_TSSI_ON:
            tssi::SetEnabled(true);

            if (elapsed > 50) {
                new_state = PWRUP_PERIPHERALS_ON;
            }
            break;

        case PWRUP_PERIPHERALS_ON:
            bindings::front_controller_en.SetHigh();
            bindings::imu_gps_en.SetHigh();

            if (elapsed > 50) {
                if (veh_can.GetRxLvCommand().has_value()) {
                    // don't actually care about the status, just that
                    // FC has come online
                    new_state = PWRUP_ACCUMULATOR_ON;
                }
            }
            break;

        case PWRUP_ACCUMULATOR_ON:
            accumulator::SetEnabled(true);

            if (accumulator::ConfirmContactorsOpen()) {
                new_state = PWRUP_MOTOR_CONTROLLER_PRECHARGING;
            }
            break;

        case PWRUP_MOTOR_CONTROLLER_PRECHARGING:
            motor_controller::SetEnabled(true);

            if (motor_controller::GetState() == motor_controller::State::ON) {
                new_state = PWRUP_SHUTDOWN_ON;
            }
            break;

        case PWRUP_SHUTDOWN_ON:
            bindings::shutdown_circuit_en.SetHigh();

            if (accumulator::IsRunning()) {
                new_state = DCDC_ON;
            }
            break;

        case DCDC_ON:
            dcdc::SetEnabled(true);

            if (elapsed > 100) {
                new_state = POWERTRAIN_PUMP_ON;
            }
            break;

        case POWERTRAIN_PUMP_ON:
            bindings::powertrain_pump1_en.SetHigh();
            bindings::powertrain_pump2_en.SetHigh();

            if (elapsed > 100) {
                new_state = POWERTRAIN_FAN_ON;
            }
            break;

        case POWERTRAIN_FAN_ON:
            fans::SetPowerSetpoint(100);

            if (fans::IsAtSetpoint() || elapsed > 10000) {
                new_state = READY_TO_DRIVE;
            }
            break;

        case READY_TO_DRIVE:
            break;

        case SHUTDOWN_DRIVER_WARNING:
            // This state's action is "sending the State over CAN" which
            // happens before the switch

            // Give the driver 30 s to stop before we make them.
            if (elapsed > 30000) {
                new_state = SHUTDOWN_PUMP_OFF;
            }
            break;

        case SHUTDOWN_PUMP_OFF:
            bindings::powertrain_pump1_en.SetLow();
            bindings::powertrain_pump2_en.SetLow();

            if (elapsed > 50) {
                new_state = SHUTDOWN_FAN_OFF;
            }
            break;

        case SHUTDOWN_FAN_OFF:
            fans::SetPowerSetpoint(0);

            if (elapsed > 50) new_state = SHUTDOWN_COMPLETE;
            break;

        case SHUTDOWN_COMPLETE:
            // there's no coming back from this
            break;
    }

    // no LVBMS yet, so we can't check for shutdown
    /*
    // Shutdown checks that can occur from multiple states
    bool check_shutdown = state == DCDC_ON || state == POWERTRAIN_PUMP_ON ||
                          state == POWERTRAIN_FAN_ON || state == READY_TO_DRIVE;

    if (check_shutdown) {
        const float kNoCurrentThresholdAmp = 0.5;  // what should this be?
        bool lost_lv_comms = false;  // this should come from a SPI heartbeat

        if (dcdc::GetVoltage() < 19.2 ||
            (lost_lv_comms && dcdc::GetAmps() < kNoCurrentThresholdAmp)) {
            new_state = SHUTDOWN_DRIVER_WARNING;
        }

        auto msg = veh_can.GetRxFcStatus();
        // remove static_cast once cangen properly handles enums
        if (msg.has_value())
            // -1 is NOT SENT by FC as of 2025/02/12. This should be
            // updated in the future once FC sends useful statuses
            if (msg->GovStatus() == static_cast<uint8_t>(-1)) {
                new_state = SHUTDOWN_PUMP_OFF;
            }
    }
    */

    // If a transition was indicated, handle it
    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

}  // namespace fsm

void check_can_flash(void) {
    auto msg = veh_can.GetRxInitiateCanFlash();

    if (msg.has_value() &&
        msg->ECU() == RxInitiateCanFlash::ECU_t::LvController) {
        bindings::SoftwareReset();
    }
}

void task_1hz(void) {
    veh_can.Send(TxLvDbcHash(generated::can::kVehDbcHash));
}

void task_10hz(void) {
    static uint8_t tx_counter = 0;

    suspension::task_10hz(veh_can);
    tssi::task_10hz();
    accumulator::task_10hz(veh_can);
    // check_can_flash(); // unused in 2025

    veh_can.Send(TxLvStatus{
        .counter = tx_counter++,
        .lv_state = fsm::state,
        .motor_controller_state = motor_controller::GetState(),
        .motor_controller_switch_closed = motor_controller::GetSwitchClosed(),
        .imd_fault = tssi::GetImdFault(),
        .bms_fault = tssi::GetBmsFault(),
    });
}

void task_100hz(void) {
    dcdc::task_100hz();
    brake_light::task_100hz(veh_can);
    motor_controller::task_100hz(veh_can);

    fsm::Update_100hz();
    fans::Update_100Hz();
}

int main(void) {
    bindings::Initialize();

    accumulator::Init();
    fans::Init();
    fsm::Init();
    motor_controller::Init();

    scheduler::register_task(task_100hz, 10);
    scheduler::register_task(task_10hz, 100);
    scheduler::register_task(task_1hz, 1000);

    scheduler::run();

    while (true) continue;

    return 0;
}
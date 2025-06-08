/// @author Blake Freer
/// @date 2025-06

#include "accumulator/accumulator.hpp"
#include "bindings.hpp"
#include "dbc_hash/dbc_hash.hpp"
#include "driver_interface/driver_interface.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "motors/motors.hpp"
#include "physical.hpp"
#include "scheduler/scheduler.h"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "thresholds.hpp"
#include "vehicle_dynamics/vehicle_dynamics.hpp"

using namespace generated::can;

VehBus veh_can_bus{bindings::veh_can_base};
PtBus pt_can_bus{bindings::pt_can_base};

namespace fsm {
using State = TxFC_Status::FcState_t;

static State state = State::START_DASHBOARD;
static uint32_t elapsed = 0;

static void Update_100Hz(void) {
    using enum State;
    using DashState = RxDashboardStatus::DashState_t;

    State new_state = state;

    accumulator::Command acc_cmd;
    motors::Command motor_cmd;
    bool speaker;
    float torque_request;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    switch (state) {
        case START_DASHBOARD: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            // wait until dashboard comes online
            auto msg = veh_can_bus.GetRxDashboardStatus();
            if (msg.has_value()) {
                new_state = WAIT_DRIVER_SELECT;
            }
        } break;

        case WAIT_DRIVER_SELECT: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto msg = veh_can_bus.GetRxDashboardStatus();
            if (msg.has_value()) {
                if (msg->DashState() == DashState::WAIT_SELECTION_ACK) {
                    vehicle_dynamics::SetProfile(msg->Profile());
                    new_state = WAIT_START_HV;
                }
            }
        } break;

        case WAIT_START_HV: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto dash = veh_can_bus.GetRxDashboardStatus();
            if (dash.has_value()) {
                if (dash->DashState() == DashState::STARTING_HV) {
                    new_state = STARTUP_HV;
                }
            }
        } break;

        case STARTUP_HV:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::RUNNING) {
                new_state = STARTUP_READY_TO_DRIVE;
            }
            break;

        case STARTUP_READY_TO_DRIVE: {
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto dash = veh_can_bus.GetRxDashboardStatus();
            if (dash.has_value()) {
                if (dash->DashState() == DashState::STARTING_MOTORS) {
                    new_state = STARTUP_MOTOR;
                }
            }
        } break;

        case STARTUP_MOTOR:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (motors::GetState() == motors::State::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (motors::GetState() == motors::State::ERROR_STARTUP) {
                new_state = ERROR_UNRECOVERABLE;
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (driver_interface::IsBrakePressed()) {
                new_state = RUNNING;
            }
            break;

        case RUNNING:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = elapsed < timeout::SPEAKER_DURATION;
            torque_request = driver_interface::GetTorqueRequest();

            if (accumulator::GetState() == accumulator::State::LOW_SOC) {
                new_state = SHUTDOWN;
            }

            if (accumulator::GetState() == accumulator::State::ERROR_FEEDBACK) {
                new_state = ERROR_UNRECOVERABLE;
            }

            if (motors::GetState() == motors::State::ERROR_RUNNING) {
                new_state = ERROR_UNRECOVERABLE;
            }
            break;

        case SHUTDOWN:
            // can this be combined with ERR_STARTUP_HV?
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = START_DASHBOARD;
            }
            break;

        case ERR_STARTUP_HV:
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = START_DASHBOARD;
            }
            break;

        case ERROR_UNRECOVERABLE:
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            break;
    }

    if (hvil_interrupt) {
        new_state = SHUTDOWN;
    }

    accumulator::SetCommand(acc_cmd);
    motors::SetCommand(motor_cmd);
    vehicle_dynamics::SetDriverTorqueRequest(torque_request);
    bindings::ready_to_drive_sig_en.Set(speaker);

    if (new_state != state) {
        state = new_state;
        elapsed = 0;
    } else {
        elapsed += 10;
    }
}
}  // namespace fsm

// Shows that the ECU is alive
void ToggleDebugLed() {
    static bool state = false;
    state = !state;
    bindings::debug_led.Set(state);
}

// Reboot the ECU. Assumes that the Rasberry Pi has pulled the BOOT pin high
// so that the ECU enters bootloader mode.
void CheckCanFlash() {
    auto msg = veh_can_bus.GetRxInitiateCanFlash();

    if (msg.has_value() &&
        msg->ECU() == RxInitiateCanFlash::ECU_t::FrontController) {
        bindings::SoftwareReset();
    }
}

void LogPedalAndSteer() {
    veh_can_bus.Send(sensors::driver::GetAppsDebugMsg());
    veh_can_bus.Send(sensors::driver::GetBppsSteerDebugMsg());
}

void UpdateErrorLeds() {
    auto error_led = veh_can_bus.GetRxLvControllerStatus();
    if (error_led.has_value()) {
        bindings::imd_fault_led_en.Set(error_led->ImdFault());
        bindings::bms_fault_led_en.Set(error_led->BmsFault());
    } else {
        // Default to lights on
        bindings::imd_fault_led_en.SetHigh();
        bindings::bms_fault_led_en.SetHigh();
    }
}

void task_10hz(void* argument) {
    (void)argument;

    ToggleDebugLed();
    UpdateErrorLeds();
    dbc_hash::Update_10Hz(veh_can_bus);
    // CheckCanFlash();  // no CAN flash in 2025

    veh_can_bus.Send(TxFC_Status{
        .inv1_status = static_cast<uint8_t>(motors::GetLeftState()),
        .inv2_status = static_cast<uint8_t>(motors::GetRightState()),
        .mi_status = motors::GetState(),
        .acc_status = accumulator::GetState(),
        .fc_state = fsm::state,
        .brake_light_enable = driver_interface::IsBrakePressed(),
        .dbc_valid = dbc_hash::IsValid(),
        .elapsed = static_cast<uint8_t>(fsm::elapsed / 100),
    });
    veh_can_bus.Send(TxFCDashboardStatus{
        .receive_config = fsm::state == TxFC_Status::FcState_t::WAIT_START_HV,
        .hv_started = accumulator::GetState() == accumulator::State::RUNNING,
        .motor_started = motors::GetState() == motors::State::RUNNING,
        .drive_started = fsm::state == TxFC_Status::FcState_t::RUNNING,
        .hv_charge_percent =
            static_cast<uint8_t>(accumulator::GetPrechargePercent()),
        .speed = 0,  // todo
    });
    veh_can_bus.Send(accumulator::GetDebugMsg());
    // LogPedalAndSteer();  // temporary, reduce bus load for testing
}

void task_100hz(void* argument) {
    (void)argument;

    sensors::driver::Update_100Hz();
    sensors::dynamics::Update_100Hz();

    fsm::Update_100Hz();

    accumulator::Update_100Hz(veh_can_bus);
    vehicle_dynamics::Update_100Hz();
    motors::Update_100Hz(pt_can_bus, veh_can_bus,
                         vehicle_dynamics::GetLeftMotorRequest(),
                         vehicle_dynamics::GetRightMotorRequest());

    veh_can_bus.Send(accumulator::GetContactorCommand());
    // pt_can_bus.Send(motors::GetLeftSetpoints());
    // pt_can_bus.Send(motors::GetRightSetpoints());
}

int main(void) {
    bindings::Initialize();

    accumulator::Init();
    motors::Init();
    vehicle_dynamics::Init();

    // todo (2026): this is always on. just tie high on PCB
    bindings::dashboard_power_en.SetHigh();

    scheduler_register_task(task_10hz, 100, nullptr);
    scheduler_register_task(task_100hz, 10, nullptr);

    scheduler_run();

    while (true) continue;

    return 0;
}

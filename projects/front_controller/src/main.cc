/// @author Blake Freer
/// @date 2025-06

#include "accumulator/accumulator.hpp"
#include "alerts/alerts.hpp"
#include "bindings.hpp"
#include "dbc_hash/dbc_hash.hpp"
#include "driver_interface/driver_interface.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "motors/motors.hpp"
#include "physical.hpp"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "thresholds.hpp"
#include "vehicle_dynamics/vehicle_dynamics.hpp"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

static const size_t STACK_SIZE_WORDS =
    2048 * 16;  // TODO: Does this need to be so large?

// higher number = higher priority
static const uint32_t PRIORITY_100HZ = 3;
static const uint32_t PRIORITY_10HZ = 2;

StaticTask_t t100hz_control_block;
StackType_t t100hz_buffer[STACK_SIZE_WORDS];

StaticTask_t t10hz_control_block;
StackType_t t10hz_buffer[STACK_SIZE_WORDS];

using namespace generated::can;

VehBus veh_can_bus{bindings::veh_can_base};
PtBus pt_can_bus{bindings::pt_can_base};

namespace fsm {
using State = TxFcStatus::State_t;

static State state = State::START_DASHBOARD;
static uint32_t elapsed = 0;

static void Update_100Hz(void) {
    using enum State;
    using DashState = RxDashStatus::State_t;

    State new_state = state;

    accumulator::Command acc_cmd = accumulator::Command::OFF;
    motors::Command motor_cmd = motors::Command::OFF;
    bool speaker = false;
    float torque_request = 0.f;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    switch (state) {
        case START_DASHBOARD: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            // wait until dashboard comes online
            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value() && msg->State() == DashState::LOGO) {
                new_state = WAIT_DRIVER_SELECT;
            } else if (elapsed > timeout::DASHBOARD_BOOT_TIME) {
                alerts::GetAlertsManager().Set(
                    alerts::FcAlert::DashboardBootTimeout);
            } else {
                // keep waiting
            }
        } break;

        case WAIT_DRIVER_SELECT: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value()) {
                if (msg->State() == DashState::WAIT_SELECTION_ACK) {
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

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value()) {
                if (dash->State() == DashState::STARTING_HV) {
                    new_state = STARTING_HV;
                }
            }
        } break;

        case STARTING_HV:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::RUNNING) {
                new_state = WAIT_START_MOTOR;
            } else if (accumulator::GetState() == accumulator::State::ERROR) {
                new_state = ERROR;
            } else {
                // waiting
            }
            break;

        case WAIT_START_MOTOR: {
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value()) {
                if (dash->State() == DashState::STARTING_MOTORS) {
                    new_state = STARTING_MOTORS;
                }
            }
        } break;

        case STARTING_MOTORS:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (motors::GetState() == motors::State::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (motors::GetState() == motors::State::ERROR) {
                new_state = ERROR;
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

        case RUNNING: {
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = elapsed < timeout::SPEAKER_DURATION;
            torque_request = driver_interface::GetTorqueRequest();

            if (accumulator::GetState() == accumulator::State::ERROR) {
                new_state = ERROR;
            }

            if (motors::GetState() == motors::State::ERROR) {
                new_state = ERROR;
            }

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value() && (dash->State() == DashState::SHUTDOWN)) {
                new_state = SHUTDOWN;
            }
        } break;

        case SHUTDOWN:
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = START_DASHBOARD;
            }
            break;

        case ERROR: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value()) {
                if (msg->State() == DashState::SHUTDOWN) {
                    new_state = SHUTDOWN;
                }
            }

        } break;
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
    static bool toggle = true;
    bindings::debug_led.Set(toggle);
    toggle = !toggle;
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

void UpdateErrorLeds() {
    auto error_led = veh_can_bus.GetRxLvStatus();
    if (error_led.has_value()) {
        bindings::imd_fault_led_en.Set(error_led->ImdFault());
        bindings::bms_fault_led_en.Set(error_led->BmsFault());
    } else {
        // Default to lights on
        bindings::imd_fault_led_en.SetHigh();
        bindings::bms_fault_led_en.SetHigh();
    }
}

void task_1hz(void* argument) {
    (void)argument;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(1000));
    }
}

void task_10hz(void* argument) {
    (void)argument;
    static uint8_t tx_counter = 0;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        ToggleDebugLed();
        UpdateErrorLeds();
        dbc_hash::Update_10Hz(veh_can_bus);
        // CheckCanFlash();  // no CAN flash in 2025. pcb needs an external
        // oscillator

        veh_can_bus.Send(TxFcStatus{
            .counter = tx_counter++,
            .state = fsm::state,
            .accumulator_state = accumulator::GetState(),
            .motor_state = motors::GetState(),
            .inv1_state = motors::GetLeftState(),
            .inv2_state =
                static_cast<TxFcStatus::Inv2State_t>(motors::GetRightState()),
            .dbc_valid = dbc_hash::IsValid(),
            .inv1_starter = static_cast<uint8_t>(motors::GetLeftStarterState()),
            .inv2_starter =
                static_cast<uint8_t>(motors::GetRightStarterState()),
        });
        veh_can_bus.Send(TxDashCommand{
            .config_received = fsm::state == fsm::State::WAIT_START_HV,
            .hv_started =
                accumulator::GetState() == accumulator::State::RUNNING,
            .motor_started = motors::GetState() == motors::State::RUNNING,
            .drive_started = fsm::state == fsm::State::RUNNING,
            .reset = fsm::state == fsm::State::START_DASHBOARD,
            .errored = fsm::state == fsm::State::ERROR,
            .hv_precharge_percent =
                static_cast<uint8_t>(accumulator::GetPrechargePercent()),
            .speed = motors::GetMph(),
            .hv_soc_percent =
                static_cast<uint8_t>(accumulator::GetSocPercent()),
        });

        veh_can_bus.Send(accumulator::GetDebugMsg());
        generated::can::TxFcAlerts alert_msg;
        alert_msg.alerts_bitfield = alerts::GetAlertsManager().GetBitfield();
        veh_can_bus.Send(alert_msg);

        veh_can_bus.Send(sensors::driver::GetAppsDebugMsg());
        veh_can_bus.Send(sensors::driver::GetBppsSteerDebugMsg());
        veh_can_bus.Send(motors::GetCounters());

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(100));
    }
}

void task_100hz(void* argument) {
    (void)argument;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        sensors::driver::Update_100Hz();
        sensors::dynamics::Update_100Hz();
        driver_interface::Update_100Hz();

        fsm::Update_100Hz();

        accumulator::Update_100Hz(veh_can_bus);
        vehicle_dynamics::Update_100Hz();
        motors::Update_100Hz(pt_can_bus, veh_can_bus,
                             vehicle_dynamics::GetLeftMotorRequest(),
                             vehicle_dynamics::GetRightMotorRequest());

        veh_can_bus.Send(accumulator::GetContactorCommand());
        veh_can_bus.Send(TxLvCommand{
            .brake_light_enable = driver_interface::IsBrakePressed(),
        });

        // Disable Inverter commands for manual debugging
        veh_can_bus.Send(TxInverterSwitchCommand{
            .close_inverter_switch = motors::GetInverterEnable(),
        });
        pt_can_bus.Send(motors::GetLeftSetpoints());
        pt_can_bus.Send(motors::GetRightSetpoints());

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(10));
    }
}

int main(void) {
    bindings::Initialize();

    alerts::GetAlertsManager().ClearAll();
    accumulator::Init();
    motors::Init();
    vehicle_dynamics::Init();

    // todo (2026): this is always on. just tie high on PCB
    bindings::dashboard_power_en.SetHigh();

    xTaskCreateStatic(task_100hz, "100HZ", STACK_SIZE_WORDS, NULL,
                      PRIORITY_100HZ, t100hz_buffer, &t100hz_control_block);

    xTaskCreateStatic(task_10hz, "10HZ", STACK_SIZE_WORDS, NULL, PRIORITY_10HZ,
                      t10hz_buffer, &t10hz_control_block);

    vTaskStartScheduler();

    while (true) continue;

    return 0;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              char* pcTaskName);

// Optional: call this periodically, e.g., from task_10hz or task_1hz

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              char* pcTaskName) {
    // Handle stack overflow (e.g., light an LED, halt, etc.)
    bindings::debug_led.SetHigh();
    macfe::periph::CanErrorHandler(&bindings::veh_can_base);
    while (1) {
        // Trap CPU here
    }
}

/// @author Blake Freer
/// @date 2025-06

#include "accumulator/accumulator.hpp"
#include "bindings.hpp"
#include "driver_interface/driver_interface.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "governor/governor.hpp"
#include "motors/motors.hpp"
#include "physical.hpp"
#include "scheduler/scheduler.h"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "vehicle_dynamics/vehicle_dynamics.hpp"

/***************************************************************
    CAN
***************************************************************/
using namespace generated::can;
using DbcHashStatus = TxFC_Status::DbcHashStatus_t;

VehBus veh_can_bus{bindings::veh_can_base};
PtBus pt_can_bus{bindings::pt_can_base};

// create a dashboard module
TxFCDashboardStatus to_dash{
    .receive_config = false,
    .hv_started = false,
    .motor_started = false,
    .drive_started = false,
    .hv_charge_percent = false,
    .speed = 0,
};

static TxFC_Status::FcState_t fc_state = TxFC_Status::FcState_t::INIT;
static bool state_machine_on_enter = true;

DbcHashStatus dbc_hash_status = DbcHashStatus::WAITING;  // make module

static void update_state_machine(void) {
    using enum TxFC_Status::FcState_t;
    using DashState = RxDashboardStatus::DashState_t;

    std::optional<TxFC_Status::FcState_t> transition = std::nullopt;

    switch (fc_state) {
        case INIT:
            transition = START_DASHBOARD;
            break;

        case START_DASHBOARD: {
            bindings::dashboard_power_en.SetHigh();

            // wait until dashboard comes alive
            auto msg = veh_can_bus.GetRxDashboardStatus();
            if (msg.has_value()) {
                transition = SYNC_HASH;
            }
        } break;

        case SYNC_HASH: {
            dbc_hash_status = DbcHashStatus::WAITING;

            auto msg = veh_can_bus.GetRxSyncDbcHash();

            if (msg.has_value()) {
                // bypass hash check for now
                dbc_hash_status = DbcHashStatus::VALID;
                transition = WAIT_DRIVER_SELECT;
                break;

                // if (msg->DbcHash() == generated::can::kVehDbcHash) {
                //     fc_status.dbc_hash_status = DbcHashStatus::VALID;
                //     transition = WAIT_DRIVER_SELECT;
                // } else {
                //     fc_status.dbc_hash_status = DbcHashStatus::INVALID;
                //     transition = ERROR_INVALID_HASH;
                // }
            }
        } break;

        case WAIT_DRIVER_SELECT: {
            auto msg = veh_can_bus.GetRxDashboardStatus();

            if (!msg.has_value()) {
                break;
            }
            if (msg->DashState() == DashState::WAIT_SELECTION_ACK) {
                vehicle_dynamics::SetProfile(msg->Profile());
                to_dash.receive_config = true;
                transition = RUNNING;
            }
        } break;

        case RUNNING:
            break;

        case ERROR_INVALID_HASH:
            // nothing to do
            break;
    }

    state_machine_on_enter = transition.has_value();
    if (state_machine_on_enter) {
        fc_state = transition.value();
    }
}

// Shows that the ECU is alive
void toggle_debug_led() {
    static bool state = false;
    state = !state;
    bindings::debug_led.Set(state);
}

// Reboot the ECU. Assumes that the Rasberry Pi has pulled the BOOT pin high
// so that the ECU enters bootloader mode.
void check_can_flash() {
    auto msg = veh_can_bus.GetRxInitiateCanFlash();

    if (msg.has_value() &&
        msg->ECU() == RxInitiateCanFlash::ECU_t::FrontController) {
        bindings::SoftwareReset();
    }
}

void log_pedal_and_steer() {
    veh_can_bus.Send(sensors::driver::GetAppsDebugMsg());
    veh_can_bus.Send(sensors::driver::GetBppsSteerDebugMsg());
}

void update_error_leds() {
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

    toggle_debug_led();
    update_error_leds();
    // log_pedal_and_steer(); // temporary, reduce bus load for testing
    // check_can_flash();  // no CAN flash in 2025

    veh_can_bus.Send(TxFC_Status{
        .gov_status = governor::GetState(),
        .inv1_status = static_cast<uint8_t>(motors::GetLeftState()),
        .inv2_status = static_cast<uint8_t>(motors::GetRightState()),
        .mi_status = motors::GetState(),
        .acc_status = accumulator::GetState(),
        .fc_state = fc_state,
        .dbc_hash_status = dbc_hash_status,
        .brake_light_enable = driver_interface::IsBrakePressed(),
        .elapsed = static_cast<uint8_t>(governor::GetElapsed() / 100),
    });
    // veh_can_bus.Send(TxInverterSwitch{.close = motors::GetInverterEnable()});
    veh_can_bus.Send(TxFCDashboardStatus{
        .receive_config = fc_state == TxFC_Status::FcState_t::RUNNING,
        .hv_started = accumulator::GetState() == accumulator::State::RUNNING,
        .motor_started = motors::GetState() == motors::State::RUNNING,
        .drive_started = governor::GetState() == governor::State::RUNNING,
        .hv_charge_percent =
            static_cast<uint8_t>(accumulator::GetPrechargePercent()),
        .speed = 0,  // todo
    });
}

void task_100hz(void* argument) {
    (void)argument;

    sensors::driver::Update_100Hz();
    sensors::dynamics::Update_100Hz();

    update_state_machine();

    auto dash_msg = veh_can_bus.GetRxDashboardStatus();
    if (dash_msg.has_value()) {
        // this should always run
        governor::Update_100Hz(accumulator::GetState(), motors::GetState(),
                               dash_msg->DashState());
    };

    accumulator::Update_100Hz(veh_can_bus, governor::GetAccumulatorCmd());

    vehicle_dynamics::Update_100Hz(driver_interface::GetTorqueRequest());

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
    governor::Init();
    motors::Init();
    vehicle_dynamics::Init();

    scheduler_register_task(task_10hz, 100, nullptr);
    scheduler_register_task(task_100hz, 10, nullptr);

    scheduler_run();

    while (true) continue;

    return 0;
}

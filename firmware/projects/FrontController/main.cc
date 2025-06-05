/// @author Blake Freer
/// @date 2025-04

#include "accumulator/accumulator.hpp"
#include "bindings.hpp"
#include "driver_interface/driver_interface.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "governor/governor.hpp"
#include "motors/motor_interface.hpp"
#include "physical.hpp"
#include "scheduler/scheduler.h"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "vehicle_dynamics/vehicle_dynamics.hpp"

/***************************************************************
    CAN
***************************************************************/
using namespace generated::can;

VehBus veh_can_bus{bindings::veh_can_base};
PtBus pt_can_bus{bindings::pt_can_base};

/***************************************************************
    Control System
***************************************************************/

using MotorIface = MotorInterface<RxAMK0_ActualValues1, RxAMK1_ActualValues1,
                                  TxAMK0_SetPoints1, TxAMK0_SetPoints1>;
MotorIface mi;
DriverInterface di;

using DbcHashStatus = TxFC_Status::DbcHashStatus_t;

// temp until modularized
DiSts di_state = DiSts::IDLE;
MiSts mi_state = MiSts::INIT;
bool brake_light_en = false;

void UpdateControls() {
    int time_ms = bindings::GetTickMs();

    governor::Update_100Hz(accumulator::GetState(), mi_state, di_state);

    auto dash_msg = veh_can_bus.GetRxDashboardStatus();
    if (!dash_msg.has_value()) return;

    // Driver Interface update
    DriverInterface::Input di_in = {
        .command = governor::GetDriverInterfaceCmd(),
        .brake_pedal_pos = sensors::driver::GetBrakePercent(),
        .dash_cmd = dash_msg->DashState(),
        .accel_pedal_pos1 = sensors::driver::GetAccelPercent1(),
        .accel_pedal_pos2 = sensors::driver::GetAccelPercent1(),
    };
    DriverInterface::Output di_out = di.Update(di_in, time_ms);
    di_state = di_out.status;  // temp

    bindings::ready_to_drive_sig_en.Set(di_out.driver_speaker);
    brake_light_en = di_out.brake_light_en;

    auto contactor_states = veh_can_bus.GetRxContactor_Feedback();
    if (!contactor_states.has_value()) {
        return;
    }

    ContactorFeedbacks fb = {
        .precharge = static_cast<ContactorFeedback>(
            contactor_states->Pack_Precharge_Feedback()),

        .negative = static_cast<ContactorFeedback>(
            contactor_states->Pack_Negative_Feedback()),

        .positive = static_cast<ContactorFeedback>(
            contactor_states->Pack_Positive_Feedback()),
    };
    accumulator::SetPackSoc(100);  // this should come from a sensor
    accumulator::Update_100Hz(governor::GetAccumulatorCmd(), fb);

    // Vehicle Dynamics update
    vehicle_dynamics::Update_100Hz(di_out.driver_torque_req);

    auto left_act = pt_can_bus.GetRxAMK0_ActualValues1();
    auto right_act = pt_can_bus.GetRxAMK1_ActualValues1();

    if (!left_act.has_value() || !right_act.has_value()) {
        return;
    }

    // Motor Interface Update
    MotorIface::Input mi_in = {
        .cmd = governor::GetMotorCmd(),
        .left_actual1 = left_act.value(),
        .right_actual1 = right_act.value(),
        .left_motor_input = vehicle_dynamics::GetLeftMotorRequest(),
        .right_motor_input = vehicle_dynamics::GetRightMotorRequest(),
    };
    MotorIface::Output mi_out = mi.Update(mi_in, time_ms);
    (void)mi_out.inverter_enable;  // unused -> inverter en signal is set in the
    // setpoint message inside MI.Update()

    mi_state = mi_out.status;

    // pt_can_bus.Send(mi_out.left_setpoints);
    // pt_can_bus.Send(mi_out.right_setpoints);
}

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
            transition = WAIT_DRIVER_SELECT;  // bypass hash check for now
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
            UpdateControls();

            to_dash.hv_started = accumulator::GetState() == AccSts::RUNNING;
            to_dash.motor_started = mi_state == MiSts::RUNNING;
            to_dash.drive_started = di_state == DiSts::RUNNING;
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

    veh_can_bus.Send(to_dash);
    veh_can_bus.Send(TxFC_Status{
        .gov_status = governor::GetState(),
        .di_status = di_state,
        .mi_status = mi_state,
        .acc_status = accumulator::GetState(),
        .fc_state = fc_state,
        .dbc_hash_status = dbc_hash_status,
        .brake_light_enable = brake_light_en,
    });
}

void task_100hz(void* argument) {
    (void)argument;

    sensors::driver::Update_100Hz();
    sensors::dynamics::Update_100Hz();

    update_state_machine();

    ContactorCommands cmd = accumulator::GetContactorCommand();
    veh_can_bus.Send(TxContactorCommand{
        .pack_positive = static_cast<bool>(cmd.positive),
        .pack_precharge = static_cast<bool>(cmd.precharge),
        .pack_negative = static_cast<bool>(cmd.negative),
    });
}

int main(void) {
    bindings::Initialize();

    scheduler_register_task(task_10hz, 100, nullptr);
    scheduler_register_task(task_100hz, 10, nullptr);

    scheduler_run();

    while (true) continue;

    return 0;
}

/// @author Blake Freer
/// @date 2025-04

#include "bindings.hpp"
#include "control-system/battery_monitor.hpp"
#include "control-system/driver_interface.hpp"
#include "control-system/governor.hpp"
#include "control-system/motor_interface.hpp"
#include "control-system/vehicle_dynamics.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/app.hpp"
#include "physical.hpp"
#include "scheduler/scheduler.h"
#include "tuning.hpp"

/***************************************************************
    CAN
***************************************************************/
using namespace generated::can;

VehBus veh_can_bus{bindings::veh_can_base};
PtBus pt_can_bus{bindings::pt_can_base};

/***************************************************************
    Objects
***************************************************************/

Pedal apps1{
    bindings::accel_pedal_sensor1,
    tuning::apps1_volt_pos_0,
    tuning::apps1_volt_pos_100,
};

Pedal apps2{
    bindings::accel_pedal_sensor2,
    tuning::apps2_volt_pos_0,
    tuning::apps2_volt_pos_100,
};

Pedal brake{
    bindings::brake_pressure_sensor,
    tuning::bpps_volt_pos_0,
    tuning::bpps_volt_pos_100,
};

SteeringWheel steering_wheel{
    bindings::steering_angle_sensor,
    tuning::steer_volt_straight,
    tuning::steer_volt_full_right,
};

/***************************************************************
    Control System
***************************************************************/

using MotorIface = MotorInterface<RxAMK0_ActualValues1, RxAMK1_ActualValues1,
                                  TxAMK0_SetPoints1, TxAMK0_SetPoints1>;
MotorIface mi;
BatteryMonitor bm;
Governor gov;
DriverInterface di;
VehicleDynamics vd{
    tuning::pedal_to_torque,
    tuning::GetProfile(generated::can::RxDashboardStatus::Profile_t::Default)};

Governor::Input gov_in{};

using DbcHashStatus = TxFC_Status::DbcHashStatus_t;
TxFC_Status fc_status{GovSts::INIT,
                      DiSts::IDLE,
                      MiSts::INIT,
                      BmSts::INIT,
                      TxFC_Status::FcState_t::INIT,
                      DbcHashStatus::WAITING};
TxContactorCommand contactor_cmd{
    static_cast<bool>(ContactorCommand::State::OPEN),
    static_cast<bool>(ContactorCommand::State::OPEN),
    static_cast<bool>(ContactorCommand::State::OPEN),
};

void UpdateControls() {
    int time_ms = bindings::GetTickMs();

    Governor::Output gov_out = gov.Update(gov_in, time_ms);

    fc_status.gov_status = gov_out.gov_sts;
    fc_status.di_status = gov_in.di_sts;
    fc_status.mi_status = gov_in.mi_sts;
    fc_status.bm_status = gov_in.bm_sts;

    float brake_position = brake.ReadPosition();

    auto dash_msg = veh_can_bus.GetRxDashboardStatus();
    if (!dash_msg.has_value()) return;

    // Driver Interface update
    DriverInterface::Input di_in = {
        .command = gov_out.di_cmd,
        .brake_pedal_pos = brake_position,
        .dash_cmd = dash_msg->DashState(),
        .accel_pedal_pos1 = apps1.ReadPosition(),
        .accel_pedal_pos2 = apps2.ReadPosition(),
    };
    DriverInterface::Output di_out = di.Update(di_in, time_ms);
    gov_in.di_sts = di_out.status;

    bindings::ready_to_drive_sig_en.Set(di_out.driver_speaker);
    veh_can_bus.Send(TxBrakeLight{.enable = di_out.brake_light_en});

    auto contactor_states = veh_can_bus.GetRxContactor_Feedback();
    if (!contactor_states.has_value()) {
        return;
    }

    BatteryMonitor::Input bm_in = {
        .cmd = gov_out.bm_cmd,
        .feedback{
            .precharge = static_cast<ContactorFeedback::State>(
                contactor_states->Pack_Precharge_Feedback()),

            .negative = static_cast<ContactorFeedback::State>(
                contactor_states->Pack_Negative_Feedback()),

            .positive = static_cast<ContactorFeedback::State>(
                contactor_states->Pack_Positive_Feedback()),
        },
        .pack_soc = 550  // temporary, should it come from sensor?
    };
    BatteryMonitor::Output bm_out = bm.Update(bm_in, time_ms);
    gov_in.bm_sts = bm_out.status;

    contactor_cmd.pack_positive = static_cast<bool>(bm_out.command.positive);
    contactor_cmd.pack_precharge = static_cast<bool>(bm_out.command.precharge);
    contactor_cmd.pack_negative = static_cast<bool>(bm_out.command.negative);

    // Vehicle Dynamics update
    VehicleDynamics::Input vd_in = {
        .driver_torque_request = di_out.driver_torque_req,
        .brake_pedal_postion = brake_position,
        .steering_angle = steering_wheel.ReadPosition(),
        // All wheel speed inputs were fixed to 0 in the old simulink model
        .wheel_speed_lr = 0 * bindings::wheel_speed_rear_left.ReadVoltage(),
        .wheel_speed_rr = 0 * bindings::wheel_speed_rear_right.ReadVoltage(),
        .wheel_speed_lf = 0 * bindings::wheel_speed_front_left.ReadVoltage(),
        .wheel_speed_rf = 0 * bindings::wheel_speed_front_right.ReadVoltage(),
        .tv_enable = false,
    };
    VehicleDynamics::Output vd_out = vd.Update(vd_in, time_ms);

    auto left_act = pt_can_bus.GetRxAMK0_ActualValues1();
    auto right_act = pt_can_bus.GetRxAMK1_ActualValues1();

    if (!left_act.has_value() || !right_act.has_value()) {
        return;
    }

    // Motor Interface Update
    MotorIface::Input mi_in = {
        .cmd = gov_out.mi_cmd,
        .left_actual1 = left_act.value(),
        .right_actual1 = right_act.value(),
        .left_motor_input = vd_out.left_motor_request,
        .right_motor_input = vd_out.right_motor_request,
    };
    MotorIface::Output mi_out = mi.Update(mi_in, time_ms);
    (void)mi_out.inverter_enable;  // unused -> inverter en signal is set in the
    // setpoint message inside MI.Update()

    gov_in.mi_sts = mi_out.status;

    // pt_can_bus.Send(mi_out.left_setpoints);
    // pt_can_bus.Send(mi_out.right_setpoints);
}

TxFCDashboardStatus to_dash{
    .receive_config = false,
    .hv_started = false,
    .motor_started = false,
    .drive_started = false,
    .hv_charge_percent = false,
    .speed = 0,
};

static TxFC_Status::FcState_t fc_state = TxFC_Status::FcState_t::INIT;
static tuning::Profile profile;
static bool state_machine_on_enter = true;

static void update_state_machine(void) {
    using enum TxFC_Status::FcState_t;
    using DashState = RxDashboardStatus::DashState_t;

    fc_status.fc_state = fc_state;

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
            fc_status.dbc_hash_status = DbcHashStatus::WAITING;

            auto msg = veh_can_bus.GetRxSyncDbcHash();

            if (msg.has_value()) {
                // bypass hash check for now
                fc_status.dbc_hash_status = DbcHashStatus::VALID;
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
                if (msg->Profile() == RxDashboardStatus::Profile_t::Tuning) {
                    transition = WAIT_RASPI_TUNING;
                } else {
                    profile = tuning::GetProfile(msg->Profile());
                    transition = RUNNING;
                }
            }
        } break;

        case WAIT_RASPI_TUNING: {
            auto msg = veh_can_bus.GetRxTuningParams();
            if (msg.has_value()) {
                profile = tuning::Profile{
                    .aggressiveness = float(msg->aggressiveness()),
                    // fill in other fields
                };
                transition = RUNNING;
            }
        } break;

        case RUNNING:
            if (state_machine_on_enter) {
                to_dash.receive_config = true;
                vd = VehicleDynamics{tuning::pedal_to_torque, profile};
            }

            UpdateControls();

            to_dash.hv_started = gov_in.bm_sts == BmSts::RUNNING;
            to_dash.motor_started = gov_in.mi_sts == MiSts::RUNNING;
            to_dash.drive_started = gov_in.di_sts == DiSts::RUNNING;
            break;

            // case ERROR_HASH_INVALID:
            //     break;

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
    veh_can_bus.Send(TxFC_apps_debug{
        .apps1_raw_volt = bindings::accel_pedal_sensor1.ReadVoltage(),
        .apps2_raw_volt = bindings::accel_pedal_sensor2.ReadVoltage(),
        .apps1_pos = apps1.ReadPosition(),
        .apps2_pos = apps2.ReadPosition(),
    });
    veh_can_bus.Send(TxFC_bpps_steer_debug{
        .bpps_raw_volt = bindings::brake_pressure_sensor.ReadVoltage(),
        .steering_raw_volt = bindings::steering_angle_sensor.ReadVoltage(),
        .bpps_pos = brake.ReadPosition(),
        .steering_pos = steering_wheel.ReadPosition(),
    });
}

void update_error_leds() {
    auto error_led = veh_can_bus.GetRxLvControllerStatus();
    if (error_led.has_value()) {
        bindings::imd_fault_led_en.Set(error_led->ImdFault());
        bindings::bms_fault_led_en.Set(error_led->BmsFault());
    }
}

void task_10hz(void* argument) {
    (void)argument;

    toggle_debug_led();
    update_error_leds();
    log_pedal_and_steer();
    // check_can_flash();  // no CAN flash in 2025

    veh_can_bus.Send(to_dash);
    veh_can_bus.Send(fc_status);
}

void task_100hz(void* argument) {
    (void)argument;

    update_state_machine();

    veh_can_bus.Send(contactor_cmd);
}

int main(void) {
    bindings::Initialize();

    scheduler_register_task(task_10hz, 100, nullptr);
    scheduler_register_task(task_100hz, 10, nullptr);

    scheduler_run();

    while (true) continue;

    return 0;
}

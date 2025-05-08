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
VehicleDynamics vd{tuning::pedal_to_torque};

Governor::Input gov_in{};

using DbcHashStatus = TxFC_Status::DbcHashStatus_t;
TxFC_Status fc_status{0, DiSts::IDLE, 0, 0, DbcHashStatus::WAITING};
TxContactorCommand contactor_cmd{
    static_cast<bool>(ContactorCommand::State::OPEN),
    static_cast<bool>(ContactorCommand::State::OPEN),
    static_cast<bool>(ContactorCommand::State::OPEN),
};

void UpdateControls() {
    int time_ms = bindings::GetTickMs();

    Governor::Output gov_out = gov.Update(gov_in, time_ms);

    fc_status.gov_status = static_cast<uint8_t>(gov_out.gov_sts);
    fc_status.di_status = gov_in.di_sts;
    fc_status.mi_status = static_cast<uint8_t>(gov_in.mi_sts);
    fc_status.bm_status = static_cast<uint8_t>(gov_in.bm_sts);

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
        .left_motor_input =
            {
                .speed_request =
                    static_cast<float>(vd_out.left_motor_speed_request),
                .torque_limit_positive = vd_out.lm_torque_limit_positive,
                .torque_limit_negative = vd_out.lm_torque_limit_negative,
            },
        .right_motor_input =
            {
                .speed_request =
                    static_cast<float>(vd_out.right_motor_speed_request),
                .torque_limit_positive = vd_out.rm_torque_limit_positive,
                .torque_limit_negative = vd_out.rm_torque_limit_negative,
            },
    };
    MotorIface::Output mi_out = mi.Update(mi_in, time_ms);
    (void)mi_out.inverter_enable;  // unused -> inverter en signal is set in the
    // setpoint message inside MI.Update()

    gov_in.mi_sts = mi_out.status;

    pt_can_bus.Send(mi_out.left_setpoints);
    pt_can_bus.Send(mi_out.right_setpoints);
}

class FrontController {
public:
    enum class State {
        INIT,
        START_DASHBOARD,
        SYNC_HASH,
        WAIT_DRIVER_SELECT,
        // WAIT_HV_START,
        // STARTING_HV,
        // WAIT_MOTOR_START,
        // STARTING_MOTOR,
        RUNNING,
        ERROR_HASH_INVALID,
    };

    FrontController(int start_time)
        : state_(State::INIT), state_enter_time_(start_time) {}

    void Update(int time_ms);

private:
    State state_;
    int state_enter_time_;
    bool on_enter = true;

    TxFCDashboardStatus to_dash{0, 0, 0, 0};
};

void FrontController::Update(int time_ms) {
    using enum State;
    using DashState = RxDashboardStatus::DashState_t;
    std::optional<State> transition = std::nullopt;
    int elapsed = time_ms - state_enter_time_;

    switch (state_) {
        case INIT:
            // this state is trivial and can be removed unless we add more
            transition = START_DASHBOARD;
            break;

        case START_DASHBOARD: {
            bindings::dashboard_power_en.SetHigh();

            // wait until dashboard comes alive
            auto msg = veh_can_bus.GetRxDashboardStatus();
            // if (msg.has_value()) transition = SYNC_HASH;
            if (msg.has_value()) transition = WAIT_DRIVER_SELECT;
        } break;

        case SYNC_HASH: {
            if (on_enter) {
                fc_status.dbc_hash_status = DbcHashStatus::WAITING;
            }

            auto msg = veh_can_bus.GetRxSyncDbcHash();

            if (msg.has_value()) {
                if (msg->DbcHash() == generated::can::kVehDbcHash) {
                    fc_status.dbc_hash_status = DbcHashStatus::VALID;
                    transition = WAIT_DRIVER_SELECT;
                } else {
                    fc_status.dbc_hash_status = DbcHashStatus::INVALID;
                    transition = ERROR_HASH_INVALID;
                }
            }
        } break;

        case WAIT_DRIVER_SELECT: {
            auto msg = veh_can_bus.GetRxDashboardStatus();

            if (msg.has_value()) {
                if (msg->DashState() == DashState::WAIT_SELECTION_ACK) {
                    to_dash.receive_config = true;
                    transition = RUNNING;
                }
            }
        } break;

        case RUNNING:
            UpdateControls();

            to_dash.hv_started = gov_in.bm_sts == BmSts::RUNNING;
            to_dash.motor_started = gov_in.mi_sts == MiSts::RUNNING;
            to_dash.drive_started = gov_in.di_sts == DiSts::RUNNING;
            break;

        case ERROR_HASH_INVALID:
            break;
    }

    veh_can_bus.Send(to_dash);

    on_enter = transition.has_value();
    if (on_enter) {
        on_enter = true;
        state_enter_time_ = time_ms;
        state_ = transition.value();
    }
}

int main(void) {
    bindings::Initialize();

    bool state = true;

    FrontController fc{bindings::GetTickMs()};

    while (true) {
        fc.Update(bindings::GetTickMs());

        auto error_led = veh_can_bus.GetRxFaultLEDs();
        if (error_led.has_value()) {
            bindings::imd_fault_led_en.Set(error_led->IMD());
            bindings::bms_fault_led_en.Set(error_led->BMS());
        }

        veh_can_bus.Send(fc_status);
        veh_can_bus.Send(contactor_cmd);

        // test pedals
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

        bindings::debug_led.Set(state);
        state = !state;

        auto msg = veh_can_bus.GetRxInitiateCanFlash();

        if (msg.has_value() &&
            msg->ECU() == RxInitiateCanFlash::ECU_t::FrontController) {
            bindings::SoftwareReset();
        }
        bindings::DelayMs(10);
    }

    return 0;
}

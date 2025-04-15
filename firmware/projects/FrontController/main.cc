/// @author Blake Freer
/// @date 2025-02

#include "bindings.hpp"
#include "control-system/battery_monitor.h"
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

// Global Governer input defined
Governor::Input gov_in{};

using DbcHashStatus = TxFC_Status::DbcHashStatus_t;
TxFC_Status fc_status{0, DiSts::IDLE, 0, 0, DbcHashStatus::WAITING};

void UpdateControls() {
    // NOTE #1: For defining inputs, I commented out inputs where I didn't know
    // what to set it to NOTE #2: Some binding values like brake_pedal_front and
    // accel_pedal are defined above, I used those and used Update(), idk if
    // thats the correct thing to do or not NOTE #3: There are some warnings for
    // type conversions, I don't know if those are mistakes from me or some
    // conversions from simulink to c++ were wrong NOTE #4: All outputs are
    // created and most of them are supposed to be sent to CAN. Assuming Blake
    // will be doing that part?

    // Capture time to use in Update call for each block
    int time_ms = bindings::GetTickMs();

    // Governer update
    // Question: do I just use the default values for the gov input at first, or
    // do I set some value?
    Governor::Output gov_out = gov.Update(gov_in, time_ms);

    fc_status.gov_status = static_cast<uint8_t>(gov_out.gov_sts);
    fc_status.di_status = gov_in.di_sts;
    fc_status.mi_status = static_cast<uint8_t>(gov_in.mi_sts);
    fc_status.bm_status = static_cast<uint8_t>(gov_in.bm_sts);

    // Driver Interface update
    DriverInterface::Input di_in = {
        .di_cmd = gov_out.di_cmd,
        .brake_pedal_pos = brake.ReadPosition(),
        .driver_button = 0,  // bindings::start_button.Read(),
        .accel_pedal_pos1 = apps1.ReadPosition(),
        .accel_pedal_pos2 = apps2.ReadPosition(),
        .steering_angle = steering_wheel.ReadPosition(),
    };
    DriverInterface::Output di_out = di.Update(di_in, time_ms);
    gov_in.di_sts = di_out.di_sts;

    // bindings::rtds_en.Set(di_out.driver_speaker);
    // bindings::brake_light_en.Set(di_out.brake_light_en);

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
                !contactor_states->Pack_Negative_Feedback()),

            .positive = static_cast<ContactorFeedback::State>(
                contactor_states->Pack_Positive_Feedback()),
        },
        .pack_soc = 550  // temporary, should it come from sensor?
    };
    BatteryMonitor::Output bm_out = bm.Update(bm_in, time_ms);
    gov_in.bm_sts = bm_out.status;

    veh_can_bus.Send(TxContactorCommand{
        .pack_positive = static_cast<bool>(bm_out.command.positive),
        .pack_precharge = static_cast<bool>(bm_out.command.precharge),
        .pack_negative = static_cast<bool>(bm_out.command.negative),
    });

    // Vehicle Dynamics update
    VehicleDynamics::Input vd_in = {
        .driver_torque_request = di_out.driver_torque_req,
        .brake_pedal_postion = di_out.brake_pedal_position,
        .steering_angle = di_out.steering_angle,
        // These are all connected to GND in simulink, assuming that means set
        // to 0
        .wheel_speed_lr = 0,  // set to wheel speed sensors later
        .wheel_speed_rr = 0,
        .wheel_speed_lf = 0,
        .wheel_speed_rf = 0,
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

int main(void) {
    bindings::Initialize();

    // Constantly check for hash to compare dbc version's between boards
    auto dbc_hash = veh_can_bus.GetRxSyncDbcHash();
    while (!dbc_hash.has_value()) {
        fc_status.dbc_hash_status = DbcHashStatus::WAITING;
        veh_can_bus.Send(fc_status);

        bindings::DelayMs(100);

        dbc_hash = veh_can_bus.GetRxSyncDbcHash();
    }

    // Error state. If hash of LVController doesn't match, constantly send
    // invalid messages back, not exiting
    if (dbc_hash->DbcHash() != generated::can::kVehDbcHash) {
        while (true) {
            fc_status.dbc_hash_status = DbcHashStatus::INVALID;
            veh_can_bus.Send(fc_status);

            bindings::DelayMs(100);
        }
    }
    fc_status.dbc_hash_status = DbcHashStatus::VALID;

    bindings::dashboard_power_en.SetHigh();

    bool state = true;

    while (true) {
        UpdateControls();
        veh_can_bus.Send(fc_status);

        // test pedals
        veh_can_bus.Send(TxFC_apps_debug{
            .apps1_raw_volt = bindings::accel_pedal_sensor1.ReadVoltage(),
            .apps2_raw_volt = bindings::accel_pedal_sensor2.ReadVoltage(),
            .apps1_pos = apps1.ReadPosition(),
            .apps2_pos = apps2.ReadPosition(),
        });
        veh_can_bus.Send(TxFC_debug{
            .fc_bpps = static_cast<uint16_t>(
                100 * bindings::brake_pressure_sensor.ReadVoltage()),
            .fc_steering_angle = static_cast<uint16_t>(
                100 * bindings::steering_angle_sensor.ReadVoltage()),
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
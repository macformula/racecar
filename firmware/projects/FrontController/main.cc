/// @author Blake Freer
/// @date 2025-02

#include <cstdint>
#include <optional>

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
#include "shared/util/mappers/linear_map.hpp"

/***************************************************************
    CAN
***************************************************************/
using namespace generated::can;

generated::can::VehBus veh_can_bus{bindings::veh_can_base};
generated::can::PtBus pt_can_bus{bindings::pt_can_base};

/***************************************************************
    Objects
***************************************************************/
using shared::util::LinearMap;

// See fc_docs/pedal_function and
auto accel_pedal_map = LinearMap<float>{0.5, -0.25};
AnalogInput accel_pedal_1{bindings::accel_pedal_sensor1, &accel_pedal_map};
AnalogInput accel_pedal_2{bindings::accel_pedal_sensor2, &accel_pedal_map};

const float kPressureRange = 2000;

// See datasheets/race_grade/RG_SPEC-0030_M_APT_G2_DTM.pdf
auto brake_pedal_front_map =
    LinearMap<float>{0.378788f * kPressureRange, -0.125f * kPressureRange};
AnalogInput brake_pedal_front{bindings::brake_pressure_sensor,
                              &brake_pedal_front_map};

// Full Left (0V) -> -1. Full right (3.3V) -> +1 --> convert
// testing: Full Left -> 0, Full Right -> +1
auto steering_wheel_map = LinearMap<float>{1.0 / 3.3, 0};
AnalogInput steering_wheel{bindings::steering_angle_sensor,
                           &steering_wheel_map};

/***************************************************************
    Control System
***************************************************************/

auto pedal_to_torque = LinearMap<float>{1.f, 0.f};

using MotorIface = MotorInterface<RxAMK0_ActualValues1, RxAMK1_ActualValues1,
                                  TxAMK0_SetPoints1, TxAMK0_SetPoints1>;
MotorIface mi;
BatteryMonitor bm;
Governor gov;
DriverInterface di;
VehicleDynamics vd{pedal_to_torque};

// Global Governer input defined
Governor::Input gov_in{};

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

    veh_can_bus.Send(TxFC_Status{
        .gov_status = static_cast<uint8_t>(gov_out.gov_sts),
        .di_status = static_cast<uint8_t>(gov_in.di_sts),
        .mi_status = static_cast<uint8_t>(gov_in.mi_sts),
        .bm_status = static_cast<uint8_t>(gov_in.bm_sts),
        .user_flag = false,  // bindings::start_button.Read(),
    });

    // Driver Interface update
    DriverInterface::Input di_in = {
        .di_cmd = gov_out.di_cmd,
        .brake_pedal_pos = brake_pedal_front.Update(),
        .driver_button = 0,  // bindings::start_button.Read(),
        .accel_pedal_pos1 = accel_pedal_1.Update(),
        .accel_pedal_pos2 = accel_pedal_2.Update(),
        .steering_angle = steering_wheel.Update()};
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
        .precharge_contactor_states = static_cast<ContactorState>(
            !contactor_states->Pack_Precharge_Feedback()),
        .pos_contactor_states = static_cast<ContactorState>(
            !contactor_states->Pack_Positive_Feedback()),
        .neg_contactor_states = static_cast<ContactorState>(
            !contactor_states->Pack_Negative_Feedback()),
        .pack_soc = 550  // temporary, should it come from sensor?
    };
    BatteryMonitor::Output bm_out = bm.Update(bm_in, time_ms);
    gov_in.bm_sts = bm_out.status;

    veh_can_bus.Send(TxContactorCommand{
        .pack_positive = static_cast<bool>(bm_out.contactor.positive),
        .pack_precharge = static_cast<bool>(bm_out.contactor.precharge),
        .pack_negative = static_cast<bool>(bm_out.contactor.negative),
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

/***************************************************************
    Dashboard FSM
***************************************************************/

enum class State {
    WAIT_FOR_DASHBOARD_ON,
    WAIT_FOR_HV_DATA,
    TURN_ON_HV,
    WAIT_FOR_MOTOR_DATA,
    TURN_ON_MOTOR,
    WAIT_FOR_BRAKES_PRESSED,
    TURN_ON_BRAKES,
    DRIVING
};

class StateMachine {
public:
    // Constructor w/ initial state and initial time
    StateMachine(int start_time)
        : state_(State::WAIT_FOR_DASHBOARD_ON),
          state_enter_time_(start_time),
          on_enter_(true) {}

    void Update(int time_ms) {
        std::optional<State> transition = std::nullopt;
        int elapsed = time_ms - state_enter_time_;

        auto msg = veh_can_bus.GetRxDashboardIndicatorStatus();

        // continously send message
        veh_can_bus.Send(TxFCDashboardStatus{
            .hv_started = hv_started,
            .motor_started = motor_started,
            .drive_started = drive_started,
        });

        // State machine logic
        switch (state_) {
            case State::WAIT_FOR_DASHBOARD_ON:
                // doesn't do anything specific in this state
                if (on_enter_) {
                }

                if (msg->dashStatus() == 0b1) {
                    transition = State::WAIT_FOR_HV_DATA;
                }
                break;

            case State::WAIT_FOR_HV_DATA:
                // wait for Dash to send indicator to start HV
                // at the same time update driver and event data with whatever
                // dash sends
                if (on_enter_) {
                }

                driver_number = msg->driverNumber();
                event_number = msg->eventNumber();

                // once we recieve indication to turn on HV, go to that state
                if (msg->startHvIndicator() == 0b1) {
                    transition = State::TURN_ON_HV;
                }
                break;

            case State::TURN_ON_HV:
                // Here we turn on the HV system and wait till that is finished.
                // Once it is finished, we tell Dash that it has finished
                if (on_enter_) {
                    //*START HV HERE*
                }

                //*CHANGE CONDITION TO CHECK IF HV HAS FINISHED STARTING*
                if (true) {
                    hv_started = true;
                    transition = State::WAIT_FOR_MOTOR_DATA;
                    break;
                }
                break;

            case State::WAIT_FOR_MOTOR_DATA:
                // waits for dash to indicator motor starting
                if (on_enter_) {
                }

                // once we recieve indication, transition to turn it on
                if (msg->startMotorIndicator() == 0b1) {
                    transition = State::TURN_ON_MOTOR;
                }
                break;

            case State::TURN_ON_MOTOR:
                // Here again, we start motor and wait for it to be turned on
                // Then we send message to dash that it has been turned on
                if (on_enter_) {
                    //*START MOTOR HERE*
                }

                //*CHANGE CONDITION TO CHECK IF MOTOR HAS STARTED*
                if (true) {
                    motor_started = true;
                    transition = State::WAIT_FOR_BRAKES_PRESSED;
                    break;
                }
                break;

            case State::WAIT_FOR_BRAKES_PRESSED:
                // Here we check if breaks are pressed, if so transition to turn
                // on the vehicle
                if (on_enter_) {
                }

                //*CHANGE THIS TO CHECK IF BREAKS WERE PRESSED*
                if (true) {
                    transition = State::TURN_ON_BRAKES;
                }
                break;

            case State::TURN_ON_BRAKES:
                // We start the vehicle, wait for it to fully turn on, once it
                // is tell dashboard that it is turned on
                if (on_enter_) {
                    //*TURN ON VEHICLE HERE*
                }

                // CHANGE TO CHECK IF VEHICLE IS ON
                if (true) {
                    drive_started = true;
                    transition = State::DRIVING;
                    break;
                }
                break;

            case State::DRIVING:
                if (on_enter_) {
                }

                break;
        }

        // Handle transition
        on_enter_ = transition.has_value();
        if (on_enter_) {
            state_enter_time_ = time_ms;
            state_ = transition.value();
        }
    }

private:
    State state_;
    int state_enter_time_;
    bool on_enter_;

    bool hv_started = false;
    bool motor_started = false;
    bool drive_started = false;

    uint8_t driver_number = 0;
    uint8_t event_number = 0;
};

/***************************************************************
    Main Function
***************************************************************/

int main(void) {
    bindings::Initialize();

    // Turn on dashboard and corresponding FSM
    bindings::dashboard_power_en.SetHigh();
    StateMachine dash_fsm(bindings::GetTickMs());

    bool state = true;

    while (true) {
        // dash_fsm.Update(bindings::GetTickMs());

        // UpdateControls();

        // bindings::debug_led.Set(state);
        // state = !state;

        // auto msg = veh_can_bus.GetRxInitiateCanFlash();

        // if (msg.has_value() &&
        //     static_cast<ECU>(msg->ECU()) == ECU::FrontController) {
        //     bindings::SoftwareReset();
        // }

        auto dashboard_leds = veh_can_bus.GetRxDashboardIndicatorStatus();

        if (dashboard_leds.has_value()) {
            bindings::imd_fault_led_en.Set(!dashboard_leds->IMDLed());
            bindings::bms_fault_led_en.Set(!dashboard_leds->BMSLed());
            // bindings::imd_fault_led_en.SetLow();
            // bindings::bms_fault_led_en.SetHigh();
        } else {
            static bool tog = false;
            bindings::imd_fault_led_en.Set(tog);
            bindings::bms_fault_led_en.Set(tog);
            tog = !tog;
        }

        bindings::DelayMs(100);
    }

    return 0;
}
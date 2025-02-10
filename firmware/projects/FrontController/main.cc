// $ make PROJECT=FrontController PLATFORM=dummy
// if get to "linking main.elf" then your code compiles.
// if fails after that, no issue

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
auto accel_pedal_map = LinearMap<double, uint16_t>{0.5, -0.25};
AnalogInput accel_pedal_1{bindings::accel_pedal_sensor1, &accel_pedal_map};
AnalogInput accel_pedal_2{bindings::accel_pedal_sensor2, &accel_pedal_map};

const float kPressureRange = 2000;

// See datasheets/race_grade/RG_SPEC-0030_M_APT_G2_DTM.pdf
auto brake_pedal_front_map = LinearMap<double, uint16_t>{
    0.378788 * kPressureRange, -0.125 * kPressureRange};
AnalogInput brake_pedal_front{bindings::brake_pressure_sensor,
                              &brake_pedal_front_map};

// Full Left (0V) -> -1. Full right (3.3V) -> +1
auto steering_wheel_map = LinearMap<double, uint16_t>{2.0 / 3.3, -1};
AnalogInput steering_wheel{bindings::steering_angle_sensor,
                           &steering_wheel_map};

/***************************************************************
    RTOS
***************************************************************/
extern "C" {
void Task_5ms(void* argument);
void Task_500ms(void* argument);
}

namespace os {
extern void TickUntil(uint32_t ticks);
extern uint32_t GetTickCount();
extern void InitializeKernel();
extern void StartKernel();
}  // namespace os

/***************************************************************
    Control System
***************************************************************/

auto pedal_to_torque = LinearMap<float, float>{1, 0};

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
    // NOTE #1: For defining inputs, I commented out inputs where I didn't know what to set it to
    // NOTE #2: Some binding values like brake_pedal_front and accel_pedal are defined above, I used those and used Update(), idk if thats the correct thing to do or not
    // NOTE #3: There are some warnings for type conversions, I don't know if those are mistakes from me or some conversions from simulink to c++ were wrong

    // Capture time to use in Update call for each block
    int time_ms = os::GetTickCount();

    // Governer update
    // Question: do I just use the default values for the gov input at first, or do I set some value?
    Governor::Output gov_out = gov.Update(gov_in, time_ms);

    // Battery Monitor update
    BatteryMonitor::Input bm_in = {
        .cmd = gov_out.bm_cmd
        // .precharge_contactor_states = ,
        // .hv_pos_contactor_states = ,
        // .hv_neg_contactor_states = ,
        // .hvil_status = ,
        // .pack_soc = 
    };
    BatteryMonitor::Output bm_out = bm.Update(bm_in, time_ms);

    // Driver Interface update
    DriverInterface::Input di_in = {
        .di_cmd = gov_out.di_cmd,
        .brake_pedal_pos = brake_pedal_front.Update(),
        // .driver_button = ,
        .accel_pedal_pos1 = accel_pedal_1.Update(),
        .accel_pedal_pos2 = accel_pedal_2.Update(),
        .steering_angle = bindings::steering_angle_sensor.ReadVoltage()
    };
    DriverInterface::Output di_out = di.Update(di_in, time_ms);

    // Vehicle Dynamics update
    VehicleDynamics::Input vd_in = {
        .driver_torque_request = di_out.driver_torque_req,
        .brake_pedal_postion = di_out.brake_pedal_position,
        .steering_angle = di_out.steering_angle,
        // These are all connected to GND in simulink, assuming that means set to 0
        .wheel_speed_lr = 0,
        .wheel_speed_rr = 0,
        .wheel_speed_lf = 0,
        .wheel_speed_rf = 0,
        .tv_enable = 0 
    };
    VehicleDynamics::Output vd_out = vd.update(vd_in, time_ms); // update should be capitalized

    // Motor Interface Update
    MotorIface::Input mi_in = {
        .cmd = gov_out.mi_cmd,
        .left_actual1 = pt_can_bus.GetRxAMK0_ActualValues1().value(),
        .right_actual1 = pt_can_bus.GetRxAMK1_ActualValues1().value(),
        .left_motor_input = {
            .speed_request = vd_out.left_motor_speed_request,
            .torque_limit_positive = vd_out.lm_torque_limit_positive,
            .torque_limit_negative = vd_out.lm_torque_limit_negative
        },
        .right_motor_input = {
            .speed_request = vd_out.right_motor_speed_request,
            .torque_limit_positive = vd_out.rm_torque_limit_positive,
            .torque_limit_negative = vd_out.rm_torque_limit_negative
        }
    };
    MotorIface::Output mi_out = mi.Update(mi_in, time_ms);

    // Craft global Governer input to use for next UpdateControls call
    gov_in = {
        .bm_sts = bm_out.status,
        .mi_sts = mi_out.status,
        .di_sts = di_out.di_sts
    };
}

/***************************************************************
    RTOS Tasks
***************************************************************/

void Task_5ms(void* arg) {
    (void)arg;  // unused

    uint32_t task_delay_ms = os::GetTickCount();

    while (true) {
        UpdateControls();

        // Repeat after another 5ms
        task_delay_ms += 5;
        os::TickUntil(task_delay_ms);
    }
}

void Task_500ms(void* arg) {
    (void)arg;  // unused

    static bool state = true;

    uint32_t task_delay_ms = os::GetTickCount();

    while (true) {
        bindings::debug_led.Set(state);
        state = !state;

        // Repeat after another 500ms
        task_delay_ms += 500;
        os::TickUntil(task_delay_ms);
    }
}

int main(void) {
    bindings::Initialize();

    os::InitializeKernel();

    os::StartKernel();

    while (true) continue;  // logic is handled by OS tasks

    return 0;
}
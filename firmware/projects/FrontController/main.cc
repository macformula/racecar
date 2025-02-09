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

Governor::Input gov_in;

void UpdateControls() {
    // cmd always from governor to module
    // // sts always from module to gov

    // MotorIface::Input mi_in{
    //     // cmd from gov
    //     .left_actual1 = pt_can_bus.GetRxAMK0_ActualValues1().value(),
    //     //.request come from DI and VD
    // };

    // int time_ms = os::GetTickCount();
    // // gov update
    // Governor::Output out = gov.Update(gov_in, tims_ms);
    // // battery_monitor
    // // di
    // DriverInterface::Input di_in{
    //     .di_cmd = out.di_cmd,
    //     .brake_pedal_pos = brake_pedal_front.Update(),
    // };
    // auto di_out = di.Update(di_in, time_ms);
    // // vd
    // // mi
    // gov_in = {.di_sts = di_out.di_sts};
}

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
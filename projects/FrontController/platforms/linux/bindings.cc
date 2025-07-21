#include "../../bindings.hpp"

#include <unistd.h>

#include <chrono>
#include <iostream>

#include "mcal/linux/analog_input.hpp"
#include "mcal/linux/can.hpp"
#include "mcal/linux/digital_input.hpp"
#include "mcal/linux/digital_output.hpp"

namespace mcal {
using namespace lnx;

// =========== CAN =========================================
CanBase veh_can_base{"vcan0", false};
CanBase pt_can_base{"vcan1", false};

// =========== Vehicle Dynamics Sensors ====================
AnalogInput suspension_travel1{"STP1"};
AnalogInput suspension_travel2{"STP2"};
AnalogInput wheel_speed_front_left{"WHEEL_SPEED_FRONT_LEFT"};    // A
AnalogInput wheel_speed_front_right{"WHEEL_SPEED_FRONT_RIGHT"};  // A
AnalogInput wheel_speed_rear_left{"WHEEL_SPEED_REAR_LEFT"};      // B
AnalogInput wheel_speed_rear_right{"WHEEL_SPEED_REAR_RIGHT"};    // B

// =========== Driver Control ==============================
AnalogInput steering_angle_sensor{"STEERING_ANGLE_SENSOR"};
AnalogInput accel_pedal_sensor1{"ACCEL_PEDAL_SENSOR1"};
AnalogInput accel_pedal_sensor2{"ACCEL_PEDAL_SENSOR2"};
AnalogInput brake_pressure_sensor{"BRAKE_PRESSURE_SENSOR"};

// =========== Status Monitors =============================
AnalogInput precharge_monitor{"PRECHARGE_MONITOR"};
DigitalInput bspd_fault{"BSPD_FAULT"};

// =========== Outputs =====================================
DigitalOutput dashboard_power_en{"DASHBOARD_POWER_EN"};
DigitalOutput imd_fault_led_en{"IMD_FAULT_LED_EN"};
DigitalOutput bms_fault_led_en{"BMS_FAULT_LED_EN"};
DigitalOutput ready_to_drive_sig_en{"READY_TO_DRIVE_SIG_EN"};
DigitalOutput debug_led{"DEBUG_LED"};

};  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::CanBase& pt_can_base = mcal::pt_can_base;

// =========== Vehicle Dynamics Sensors ====================
shared::periph::AnalogInput& suspension_travel1 = mcal::suspension_travel1;
shared::periph::AnalogInput& suspension_travel2 = mcal::suspension_travel2;
shared::periph::AnalogInput& wheel_speed_front_left =
    mcal::wheel_speed_front_left;  // A
shared::periph::AnalogInput& wheel_speed_front_right =
    mcal::wheel_speed_front_right;  // A
shared::periph::AnalogInput& wheel_speed_rear_left =
    mcal::wheel_speed_rear_left;  // B
shared::periph::AnalogInput& wheel_speed_rear_right =
    mcal::wheel_speed_rear_right;  // B

// =========== Driver Control ==============================
shared::periph::AnalogInput& steering_angle_sensor =
    mcal::steering_angle_sensor;
shared::periph::AnalogInput& accel_pedal_sensor1 = mcal::accel_pedal_sensor1;
shared::periph::AnalogInput& accel_pedal_sensor2 = mcal::accel_pedal_sensor2;
shared::periph::AnalogInput& brake_pressure_sensor =
    mcal::brake_pressure_sensor;

// =========== Status Monitors =============================
shared::periph::AnalogInput& precharge_monitor = mcal::precharge_monitor;
shared::periph::DigitalInput& bspd_fault = mcal::bspd_fault;

// =========== Outputs =====================================
shared::periph::DigitalOutput& dashboard_power_en = mcal::dashboard_power_en;
shared::periph::DigitalOutput& imd_fault_led_en = mcal::imd_fault_led_en;
shared::periph::DigitalOutput& bms_fault_led_en = mcal::bms_fault_led_en;
shared::periph::DigitalOutput& ready_to_drive_sig_en =
    mcal::ready_to_drive_sig_en;
shared::periph::DigitalOutput& debug_led = mcal::debug_led;

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
    mcal::veh_can_base.Setup();
    mcal::pt_can_base.Setup();
}

int GetTickMs() {
    using namespace std::chrono;
    long now =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();
    static long start = now;  // only set on first call

    return now - start;
}

void DelayMs(int ms) {
    usleep(ms * 1000);
}

void SoftwareReset() {
    std::cout << "Performing software reset. Will not proceed." << std::endl;
    while (true) continue;
}

}  // namespace bindings

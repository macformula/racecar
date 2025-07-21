#include "../../bindings.hpp"

#include "mcal/linux/analog_input.hpp"
#include "mcal/linux/can.hpp"
#include "mcal/linux/digital_input.hpp"
#include "mcal/linux/digital_output.hpp"
#include "mcal/linux/pwm_output.hpp"

namespace mcal {
using namespace lnx;
// Tractive System Status Indicator (2025 Rule EV.5.11.5)
DigitalOutput tssi_en{"TSSI_EN"};
DigitalOutput tssi_red_signal{"TSSI_RED_SIGNAL"};
DigitalOutput tssi_green_signal{"TSSI_GREEN_SIGNAL"};
DigitalInput imd_fault{"IMD_FAULT"};
DigitalInput bms_fault{"BMS_FAULT"};

// Powertrain Cooling
DigitalOutput powertrain_pump1_en{"POWERTRAIN_PUMP1_EN"};
DigitalOutput powertrain_pump2_en{"POWERTRAIN_PUMP2_EN"};
DigitalOutput powertrain_fan1_en{"POWERTRAIN_FAN1_EN"};
DigitalOutput powertrain_fan2_en{"POWERTRAIN_FAN2_EN"};
PWMOutput powertrain_fan_pwm{"POWERTRAIN_FAN_PWM"};  // macfe by both fans

// Motor Controller (i.e. Inverters)
DigitalOutput motor_controller_en{"MOTOR_CONTROLLER_EN"};
DigitalOutput motor_ctrl_precharge_en{"MOTOR_CTRL_PRECHARGE_EN"};
DigitalOutput motor_ctrl_switch_en{"MOTOR_CTRL_SWITCH_EN"};

// Subsystems
DigitalOutput accumulator_en{"ACCUMULATOR_EN"};
DigitalOutput front_controller_en{"FRONT_CONTROLLER_EN"};
DigitalOutput imu_gps_en{"IMU_GPS_EN"};
DigitalOutput shutdown_circuit_en{"SHUTDOWN_CIRCUIT_EN"};

// DCDC System & Measurement
DigitalOutput dcdc_en{"DCDC_EN"};
DigitalOutput dcdc_sense_select{"DCDC_SENSE_SELECT"};
AnalogInput dcdc_sense{"DCDC_SENSE"};

// Other IO
DigitalOutput brake_light_en{"BRAKE_LIGHT_EN"};
AnalogInput suspension_travel3{"SUSPENSION_TRAVEL3"};
AnalogInput suspension_travel4{"SUSPENSION_TRAVEL4"};
CanBase veh_can_base{"vcan0", false};
}  // namespace mcal

namespace bindings {

// Tractive System Status Indicator (2025 Rule EV.5.11.5)
macfe::periph::DigitalOutput& tssi_en = mcal::tssi_en;
macfe::periph::DigitalOutput& tssi_red_signal = mcal::tssi_red_signal;
macfe::periph::DigitalOutput& tssi_green_signal = mcal::tssi_green_signal;
macfe::periph::DigitalInput& imd_fault = mcal::imd_fault;
macfe::periph::DigitalInput& bms_fault = mcal::bms_fault;

// Powertrain Cooling
macfe::periph::DigitalOutput& powertrain_pump1_en = mcal::powertrain_pump1_en;
macfe::periph::DigitalOutput& powertrain_pump2_en = mcal::powertrain_pump2_en;
macfe::periph::DigitalOutput& powertrain_fan1_en = mcal::powertrain_fan1_en;
macfe::periph::DigitalOutput& powertrain_fan2_en = mcal::powertrain_fan2_en;
macfe::periph::PWMOutput& powertrain_fan_pwm =
    mcal::powertrain_fan_pwm;  // macfe by both fans

// Motor Controller (i.e. Inverters)
macfe::periph::DigitalOutput& motor_controller_en = mcal::motor_controller_en;
macfe::periph::DigitalOutput& motor_ctrl_precharge_en =
    mcal::motor_ctrl_precharge_en;
macfe::periph::DigitalOutput& motor_ctrl_switch_en = mcal::motor_ctrl_switch_en;

// Subsystems
macfe::periph::DigitalOutput& accumulator_en = mcal::accumulator_en;
macfe::periph::DigitalOutput& front_controller_en = mcal::front_controller_en;
macfe::periph::DigitalOutput& imu_gps_en = mcal::imu_gps_en;
macfe::periph::DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

// DCDC System & Measurement
macfe::periph::DigitalOutput& dcdc_en = mcal::dcdc_en;
macfe::periph::DigitalOutput& dcdc_sense_select = mcal::dcdc_sense_select;
macfe::periph::AnalogInput& dcdc_sense = mcal::dcdc_sense;

// Other IO
macfe::periph::DigitalOutput& brake_light_en = mcal::brake_light_en;
macfe::periph::AnalogInput& suspension_travel3 = mcal::suspension_travel3;
macfe::periph::AnalogInput& suspension_travel4 = mcal::suspension_travel4;
macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    std::cout << "Initializing Linux platform for LVController" << std::endl;

    mcal::veh_can_base.Setup();
}

int GetTick() {
    using namespace std::chrono;
    long now =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();
    static long start = now;  // only set on first call

    return now - start;
}

void DelayMS(uint32_t ms) {
    usleep(ms * 1000);
}

void SoftwareReset() {
    std::cout << "Performing software reset. Will not proceed." << std::endl;
    while (true) continue;
}

}  // namespace bindings
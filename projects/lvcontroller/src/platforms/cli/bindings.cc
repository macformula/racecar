/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "mcal/cli/analog_input.hpp"
#include "mcal/cli/can.hpp"
#include "mcal/cli/gpio.hpp"
#include "mcal/cli/pwm.hpp"
#include "periph/analog_input.hpp"
#include "periph/can.hpp"
#include "periph/gpio.hpp"

// clang-format off
namespace mcal {
using namespace mcal::cli;

// Tractive System Status Indicator
DigitalOutput tssi_en{"TSSI_EN"} ;
DigitalOutput tssi_red_signal{"TSSI_RED_SIGNAL"};
DigitalOutput tssi_green_signal{"TSSI_GREEN_SIGNAL"};
DigitalInput imd_fault{"IMD_FAULT"};
DigitalInput bms_fault{"BMS_FAULT"};

// Powertrain Cooling
DigitalOutput powertrain_pump1_en{"POWERTRAIN_PUMP1_EN"};
DigitalOutput powertrain_pump2_en{"POWERTRAIN_PUMP2_EN"};
DigitalOutput powertrain_fan1_en{"POWERTRAIN_FAN1_EN"};
DigitalOutput powertrain_fan2_en{"POWERTRAIN_FAN2_EN"};
PWMOutput powertrain_fan_pwm{"POWERTRAIN_FAN_PWM"};

// Motor Controller (i.e. Inverters)
DigitalOutput motor_controller_en{"MOTOR_CONTROLLER_EN"};
DigitalOutput motor_ctrl_precharge_en{"MOTOR_CTRL_PRECHARGE_EN"};
DigitalOutput motor_ctrl_switch_en{"MOTOR_CTRL_SWITCH_EN"};

// Subsystems
DigitalOutput accumulator_en{"ACCUMULATOR_EN"};
DigitalOutput front_controller_en{"FRONT_CONTROLLER_EN"};
DigitalOutput imu_gps_en{"IMU_GPS_EN"};
DigitalOutput shutdown_circuit_en{"SHUTDOWN_CIRCUIT_EN"};

// DCDC System  Measurement;
DigitalOutput dcdc_en{"DCDC_EN"};
DigitalOutput dcdc_sense_select{"DCDC_SENSE_SELECT"};
AnalogInput dcdc_sense{"DCDC_SENSE"};

// HSD Sensing
AnalogInput hsd1_isense{"HSD1_ISENSE"};
DigitalOutput hsd1_isense_en{"HSD1_ISENSE_EN"};
DigitalOutput hsd1_sel0{"HSD1_SEL0"};
DigitalOutput hsd1_sel1{"HSD1_SEL1"};
hsd::HSD4Channel hsd1{hsd1_isense, hsd1_isense_en, hsd1_sel0, hsd1_sel1};

AnalogInput hsd2_isense{"HSD2_ISENSE"};
DigitalOutput hsd2_isense_en{"HSD2_ISENSE_EN"};
DigitalOutput hsd2_sel{"HSD2_SEL"};
hsd::HSD2Channel hsd2{hsd2_isense, hsd2_isense_en, hsd2_sel};

AnalogInput hsd3_isense{"HSD3_ISENSE"};
DigitalOutput hsd3_isense_en{"HSD3_ISENSE_EN"};
DigitalOutput hsd3_sel{"HSD3_SEL"};
hsd::HSD2Channel hsd3{hsd3_isense, hsd3_isense_en, hsd3_sel};

AnalogInput hsd4_isense{"HSD4_ISENSE"};
DigitalOutput hsd4_isense_en{"HSD4_ISENSE_EN"};
DigitalOutput hsd4_sel{"HSD4_SEL"};
hsd::HSD2Channel hsd4{hsd4_isense, hsd4_isense_en, hsd4_sel};

AnalogInput hsd5_isense{"HSD5_ISENSE"};
DigitalOutput hsd5_isense_en{"HSD5_ISENSE_EN"};
DigitalOutput hsd5_sel{"HSD5_SEL"};
hsd::HSD2Channel hsd5{hsd5_isense, hsd5_isense_en, hsd5_sel};

AnalogInput hsd6_isense{"HSD6_ISENSE"};
DigitalOutput hsd6_isense_en{"HSD6_ISENSE_EN"};
DigitalOutput hsd6_sel{"HSD6_SEL"};
hsd::HSD2Channel hsd6{hsd6_isense, hsd6_isense_en, hsd6_sel};

// Other IO
DigitalOutput brake_light_en{"BRAKE_LIGHT_EN"};
AnalogInput suspension_travel3{"SUSPENSION_TRAVEL3"};
AnalogInput suspension_travel4{"SUSPENSION_TRAVEL4"};
CanBase veh_can_base{"VEH_CAN_BASE"};
}  // namespace mcal


namespace bindings {
    using namespace macfe::periph;

// Tractive System Status Indicator
DigitalOutput& tssi_en = mcal::tssi_en;
DigitalOutput& tssi_red_signal = mcal::tssi_red_signal;
DigitalOutput& tssi_green_signal = mcal::tssi_green_signal;
DigitalInput& imd_fault = mcal::imd_fault;
DigitalInput& bms_fault = mcal::bms_fault;

// Powertrain Cooling
DigitalOutput& powertrain_pump1_en = mcal::powertrain_pump1_en;
DigitalOutput& powertrain_pump2_en = mcal::powertrain_pump2_en;
DigitalOutput& powertrain_fan1_en = mcal::powertrain_fan1_en;
DigitalOutput& powertrain_fan2_en = mcal::powertrain_fan2_en;
PWMOutput& powertrain_fan_pwm = mcal::powertrain_fan_pwm;

// Motor Controller (i.e. Inverters)
DigitalOutput& motor_controller_en = mcal::motor_controller_en;
DigitalOutput& motor_ctrl_precharge_en = mcal::motor_ctrl_precharge_en;
DigitalOutput& motor_ctrl_switch_en = mcal::motor_ctrl_switch_en;

// Subsystems
DigitalOutput& accumulator_en = mcal::accumulator_en;
DigitalOutput& front_controller_en = mcal::front_controller_en;
DigitalOutput& imu_gps_en = mcal::imu_gps_en;
DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

// DCDC System & Measurement = mcal::Measurement
DigitalOutput& dcdc_en = mcal::dcdc_en;
DigitalOutput& dcdc_sense_select = mcal::dcdc_sense_select;
AnalogInput& dcdc_sense = mcal::dcdc_sense;

// HSD Sensing
AnalogInput& hsd1_isense = mcal::hsd1_isense;
DigitalOutput& hsd1_isense_en = mcal::hsd1_isense_en;
DigitalOutput& hsd1_sel0 = mcal::hsd1_sel0;
DigitalOutput& hsd1_sel1 = mcal::hsd1_sel1;

AnalogInput& hsd2_isense = mcal::hsd2_isense;
DigitalOutput& hsd2_isense_en = mcal::hsd2_isense_en;
DigitalOutput& hsd2_sel = mcal::hsd2_sel;

AnalogInput& hsd3_isense = mcal::hsd3_isense;
DigitalOutput& hsd3_isense_en = mcal::hsd3_isense_en;
DigitalOutput& hsd3_sel = mcal::hsd3_sel;

AnalogInput& hsd4_isense = mcal::hsd4_isense;
DigitalOutput& hsd4_isense_en = mcal::hsd4_isense_en;
DigitalOutput& hsd4_sel = mcal::hsd4_sel;

AnalogInput& hsd5_isense = mcal::hsd5_isense;
DigitalOutput& hsd5_isense_en = mcal::hsd5_isense_en;
DigitalOutput& hsd5_sel = mcal::hsd5_sel;

AnalogInput& hsd6_isense = mcal::hsd6_isense;
DigitalOutput& hsd6_isense_en = mcal::hsd6_isense_en;
DigitalOutput& hsd6_sel = mcal::hsd6_sel;

hsd::HSD4Channel& hsd1 = mcal::hsd1;
hsd::HSD2Channel& hsd2 = mcal::hsd2;
hsd::HSD2Channel& hsd3 = mcal::hsd3;
hsd::HSD2Channel& hsd4 = mcal::hsd4;
hsd::HSD2Channel& hsd5 = mcal::hsd5;
hsd::HSD2Channel& hsd6 = mcal::hsd6;

// Other IO
DigitalOutput& brake_light_en = mcal::brake_light_en;
AnalogInput& suspension_travel3 = mcal::suspension_travel3;
AnalogInput& suspension_travel4 = mcal::suspension_travel4;
CanBase& veh_can_base = mcal::veh_can_base;  // clang-format on

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
    mcal::veh_can_base.Setup();
}

long long GetEpochTime() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}
int GetTick() {
    static long long start_time = GetEpochTime();
    return static_cast<int>(GetEpochTime() - start_time);
}

void DelayMS(uint32_t milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void SoftwareReset() {
    std::cout << "Simulating software reset" << std::endl;
    while (true) {
        continue;
    }
}

}  // namespace bindings

/// @author Blake Freer
/// @date 2023-12-25

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "../../bindings.hpp"
#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/can.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "mcal/cli/periph/pwm.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/util/mappers/identity.hpp"


// clang-format off
namespace mcal {
using namespace mcal::cli::periph;

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
DigitalOutput raspberry_pi_en{"RASPBERRY_PI_EN"};
DigitalOutput shutdown_circuit_en{"SHUTDOWN_CIRCUIT_EN"};

// DCDC System  Measurement;
DigitalOutput dcdc_en{"DCDC_EN"};
DigitalOutput dcdc_sense_select{"DCDC_SENSE_SELECT"};
AnalogInput dcdc_sense{"DCDC_SENSE"};

// Other IO
DigitalOutput brake_light_en{"BRAKE_LIGHT_EN"};
AnalogInput suspension_travel3{"SUSPENSION_TRAVEL3"};
AnalogInput suspension_travel4{"SUSPENSION_TRAVEL4"};
CanBase veh_can_base{"VEH_CAN_BASE"};
}  // namespace mcal


namespace bindings {
    using namespace shared::periph;

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
DigitalOutput& raspberry_pi_en = mcal::raspberry_pi_en;
DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

// DCDC System & Measurement = mcal::Measurement
DigitalOutput& dcdc_en = mcal::dcdc_en;
DigitalOutput& dcdc_sense_select = mcal::dcdc_sense_select;
AnalogInput& dcdc_sense = mcal::dcdc_sense;

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
    std::cout << "entering software test" << std::endl;
    while (true) {
        continue;
    }
}

}  // namespace bindings
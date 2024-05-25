/// @author Blake Freer
/// @date 2023-12-25
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include "mcal/raspi/periph/can.h"
#include "mcal/raspi/periph/gpio.h"
#include "mcal/raspi/periph/pwm.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/identity.h"
#include "validation/sil/sil_client.h"

const std::string ecu_name = "LvController";
const std::string server_addr = "localhost:31522";

namespace val {
using namespace sil;
SilClient sil_client(server_addr);
}  // namespace val

namespace mcal {
using namespace mcal::raspi::periph;
DigitalOutput tsal_en = DigitalOutput{
    ecu_name,
    "TsalEn",
    val::sil_client,
};
DigitalOutput raspberry_pi_en = DigitalOutput{
    ecu_name,
    "RaspiEn",
    val::sil_client,
};
DigitalOutput front_controller_en = DigitalOutput{
    ecu_name,
    "FrontControllerEn",
    val::sil_client,
};
DigitalOutput speedgoat_en = DigitalOutput{
    ecu_name,
    "SpeedgoatEn",
    val::sil_client,
};
DigitalOutput accumulator_en = DigitalOutput{
    ecu_name,
    "AccumulatorEn",
    val::sil_client,
};
DigitalOutput motor_ctrl_precharge_en = DigitalOutput{
    ecu_name,
    "MotorControllerPrechargeEn",
    val::sil_client,
};
DigitalOutput motor_ctrl_en = DigitalOutput{
    ecu_name,
    "MotorControllerEn",
    val::sil_client,
};
DigitalOutput imu_gps_en = DigitalOutput{
    ecu_name,
    "ImuGpsEn",
    val::sil_client,
};
DigitalOutput shutdown_circuit_en = DigitalOutput{
    ecu_name,
    "ShutdownCircuitEn",
    val::sil_client,
};
DigitalOutput inverter_switch_en = DigitalOutput{
    ecu_name,
    "InverterSwitchEn",
    val::sil_client,
};

DigitalOutput dcdc_en = DigitalOutput{
    ecu_name,
    "DcdcEn",
    val::sil_client,
};
DigitalInput dcdc_valid = DigitalInput{
    ecu_name,
    "DcdcValid",
    val::sil_client,
};
DigitalOutput dcdc_led_en = DigitalOutput{
    ecu_name,
    "DcdcLedEn",
    val::sil_client,
};
DigitalOutput powertrain_fan_en = DigitalOutput{
    ecu_name,
    "PowertrainFanEn",
    val::sil_client,
};
DigitalOutput powertrain_pump_en = DigitalOutput{
    ecu_name,
    "PowertrainPumpEn",
    val::sil_client,
};
PWMOutput powertrain_fan_pwm = PWMOutput{"PowertrainFanPwm"};

DigitalInput glvms_disable = DigitalInput{
    ecu_name,
    "GlvmsDisable",
    val::sil_client,
};

CanBase veh_can_base = CanBase{"vcan0"};
}  // namespace mcal

namespace bindings {
// clang-format off
shared::periph::DigitalOutput& tsal_en = mcal::tsal_en;
shared::periph::DigitalOutput& raspberry_pi_en = mcal::raspberry_pi_en;
shared::periph::DigitalOutput& front_controller_en = mcal::front_controller_en;
shared::periph::DigitalOutput& speedgoat_en = mcal::speedgoat_en;
shared::periph::DigitalOutput& accumulator_en = mcal::accumulator_en;
shared::periph::DigitalOutput& motor_ctrl_precharge_en = mcal::motor_ctrl_precharge_en;
shared::periph::DigitalOutput& motor_ctrl_en = mcal::motor_ctrl_en;
shared::periph::DigitalOutput& imu_gps_en = mcal::imu_gps_en;
shared::periph::DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;
shared::periph::DigitalOutput& inverter_switch_en = mcal::inverter_switch_en;
shared::periph::DigitalOutput& dcdc_en = mcal::dcdc_en;
shared::periph::DigitalInput& dcdc_valid = mcal::dcdc_valid;
shared::periph::DigitalOutput& dcdc_led_en = mcal::dcdc_led_en;
shared::periph::DigitalOutput& powertrain_fan_en = mcal::powertrain_fan_en;
shared::periph::DigitalOutput& powertrain_pump_en = mcal::powertrain_pump_en;
shared::periph::PWMOutput& powertrain_fan_pwm = mcal::powertrain_fan_pwm;

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
// clang-format on

void Initialize() {
    std::cout << "Initializing SIL..." << std::endl;
    std::cout << "Using address: " << server_addr << std::endl;

    val::sil_client.Connect();

    mcal::veh_can_base.Setup();
    mcal::tsal_en.Register();
    mcal::raspberry_pi_en.Register();
    mcal::front_controller_en.Register();
    mcal::speedgoat_en.Register();
    mcal::accumulator_en.Register();
    mcal::motor_ctrl_precharge_en.Register();
    mcal::motor_ctrl_en.Register();
    mcal::imu_gps_en.Register();
    mcal::shutdown_circuit_en.Register();
    mcal::inverter_switch_en.Register();
    mcal::dcdc_en.Register();
    mcal::dcdc_valid.Register();
    mcal::dcdc_led_en.Register();
    mcal::powertrain_fan_en.Register();
    mcal::powertrain_pump_en.Register();
    mcal::glvms_disable.Register();

    auto start_time = std::chrono::steady_clock::now();

    // Let glvms go high, then low before starting
    std::cout << "waiting for glvms disable to go high" << std::endl;
    while (mcal::glvms_disable.Read());
    std::cout << "waiting for glvms disable to go low" << std::endl;
    while (!mcal::glvms_disable.Read()) {
        auto current_time = std::chrono::steady_clock::now();
        auto time_since =
            std::chrono::duration<double>(current_time - start_time).count();
        std::cout << "time waiting: " << std::to_string(time_since)
                  << std::endl;
    }

    std::cout << "starting lv controller app" << std::endl;
}

void DelayMS(uint32_t milliseconds) {
    std::cout << "[Delaying for " << milliseconds << " milliseconds]"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
}  // namespace bindings
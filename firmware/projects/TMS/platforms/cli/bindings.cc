#include "../../bindings.hpp"

#include <iostream>

#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/can.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "mcal/cli/periph/pwm.hpp"

namespace mcal {
using namespace cli::periph;

AnalogInput temp_sensor_adc_1("Temp Sensor 1");
AnalogInput temp_sensor_adc_2("Temp Sensor 2");
AnalogInput temp_sensor_adc_3("Temp Sensor 3");
AnalogInput temp_sensor_adc_4("Temp Sensor 4");
AnalogInput temp_sensor_adc_5("Temp Sensor 5");
AnalogInput temp_sensor_adc_6("Temp Sensor 6");

PWMOutput fan_controller_pwm("Fan");
DigitalOutput debug_led_green("Green LED");
DigitalOutput debug_led_red("Red LED");

CanBase veh_can_base("Vehicle CAN");

}  // namespace mcal

namespace bindings {

shared::periph::AnalogInput& temp_sensor_adc_1 = mcal::temp_sensor_adc_1;
shared::periph::AnalogInput& temp_sensor_adc_2 = mcal::temp_sensor_adc_2;
shared::periph::AnalogInput& temp_sensor_adc_3 = mcal::temp_sensor_adc_3;
shared::periph::AnalogInput& temp_sensor_adc_4 = mcal::temp_sensor_adc_4;
shared::periph::AnalogInput& temp_sensor_adc_5 = mcal::temp_sensor_adc_5;
shared::periph::AnalogInput& temp_sensor_adc_6 = mcal::temp_sensor_adc_6;

shared::periph::PWMOutput& fan_controller_pwm = mcal::fan_controller_pwm;
shared::periph::DigitalOutput& debug_led_green = mcal::debug_led_green;
shared::periph::DigitalOutput& debug_led_red = mcal::debug_led_red;

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    std::cout << "Initializing TMS for CLI" << std::endl;
}

uint32_t GetCurrentTimeMs() {
    using namespace std::chrono;
    static auto start_time = system_clock::now();
    auto current_time = system_clock::now();
    return duration_cast<milliseconds>(current_time - start_time).count();
}

void SoftwareReset() {
    std::cout << "Entering SoftwareReset. Will not proceed." << std::endl;
    while (true) continue;
}

}  // namespace bindings
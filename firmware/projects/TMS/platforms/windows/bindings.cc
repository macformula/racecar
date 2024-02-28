/// @author Blake Freer
/// @date 2023-12-25

#include <iostream>
#include <string>

#include "mcal/windows/periph/adc.h"
#include "mcal/windows/periph/gpio.h"
#include "mcal/windows/periph/pwm.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"

namespace mcal {
periph::ADCInput temp_sensor_adc_1{"Temperature Sensor 1"};
periph::ADCInput temp_sensor_adc_2{"Temperature Sensor 2"};
periph::ADCInput temp_sensor_adc_3{"Temperature Sensor 3"};
periph::ADCInput temp_sensor_adc_4{"Temperature Sensor 4"};
periph::ADCInput temp_sensor_adc_5{"Temperature Sensor 5"};
periph::ADCInput temp_sensor_adc_6{"Temperature Sensor 6"};
periph::PWMOutput fan_controller_pwm{"Fan Controller"};
periph::DigitalOutput debug_do_green{"Debug: Green"};
periph::DigitalOutput debug_do_red{"Debug: Red"};
}  // namespace mcal

namespace bindings {

const shared::periph::ADCInput& temp_sensor_adc_1 = mcal::temp_sensor_adc_1;
const shared::periph::ADCInput& temp_sensor_adc_2 = mcal::temp_sensor_adc_2;
const shared::periph::ADCInput& temp_sensor_adc_3 = mcal::temp_sensor_adc_3;
const shared::periph::ADCInput& temp_sensor_adc_4 = mcal::temp_sensor_adc_4;
const shared::periph::ADCInput& temp_sensor_adc_5 = mcal::temp_sensor_adc_5;
const shared::periph::ADCInput& temp_sensor_adc_6 = mcal::temp_sensor_adc_6;
const shared::periph::PWMOutput& fan_controller_pwm = mcal::fan_controller_pwm;
const shared::periph::DigitalOutput& debug_do_green = mcal::debug_do_green;
const shared::periph::DigitalOutput& debug_do_red = mcal::debug_do_red;

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

void Log(std::string message) {
    std::cout << "(Log) " << message << std::endl;
}

}  // namespace bindings

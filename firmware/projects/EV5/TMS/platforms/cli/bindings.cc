/// @author Blake Freer
/// @date 2023-12-25

#include <iostream>
#include <string>

#include "mcal/cli/periph/adc.h"
#include "mcal/cli/periph/gpio.h"
#include "mcal/cli/periph/pwm.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"

namespace mcal {
using namespace cli::periph;

ADCInput temp_sensor_adc_1{"Temperature Sensor 1"};
ADCInput temp_sensor_adc_2{"Temperature Sensor 2"};
ADCInput temp_sensor_adc_3{"Temperature Sensor 3"};
ADCInput temp_sensor_adc_4{"Temperature Sensor 4"};
ADCInput temp_sensor_adc_5{"Temperature Sensor 5"};
ADCInput temp_sensor_adc_6{"Temperature Sensor 6"};

PWMOutput fan_controller_pwm{"Fan Controller"};
DigitalOutput debug_do_blue{"Debug: Blue"};
DigitalOutput debug_do_red{"Debug: Red"};

}  // namespace mcal

namespace bindings {

const shared::periph::ADCInput& temp_sensor_adc_1 = mcal::temp_sensor_adc_1;
const shared::periph::ADCInput& temp_sensor_adc_2 = mcal::temp_sensor_adc_2;
const shared::periph::ADCInput& temp_sensor_adc_3 = mcal::temp_sensor_adc_3;
const shared::periph::ADCInput& temp_sensor_adc_4 = mcal::temp_sensor_adc_4;
const shared::periph::ADCInput& temp_sensor_adc_5 = mcal::temp_sensor_adc_5;
const shared::periph::ADCInput& temp_sensor_adc_6 = mcal::temp_sensor_adc_6;
const shared::periph::PWMOutput& fan_controller_pwm = mcal::fan_controller_pwm;
const shared::periph::DigitalOutput& debug_do_blue = mcal::debug_do_blue;
const shared::periph::DigitalOutput& debug_do_red = mcal::debug_do_red;

void Initialize() {
    std::cout << "Initializing CLI..." << std::endl;
}

void Log(std::string message) {
    std::cout << "(Log) " << message << std::endl;
}

}  // namespace bindings

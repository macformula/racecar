/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <iostream>
#include <string>

namespace bindings {

mcal::periph::ADCInput temp_sensor_adc_1{"Temperature Sensor 1"};
mcal::periph::ADCInput temp_sensor_adc_2{"Temperature Sensor 2"};
mcal::periph::ADCInput temp_sensor_adc_3{"Temperature Sensor 3"};
mcal::periph::ADCInput temp_sensor_adc_4{"Temperature Sensor 4"};
mcal::periph::ADCInput temp_sensor_adc_5{"Temperature Sensor 5"};
mcal::periph::ADCInput temp_sensor_adc_6{"Temperature Sensor 6"};

mcal::periph::PWMOutput fan_controller_pwm{"Fan Controller"};

mcal::periph::DigitalOutput debug_do_green{"Debug: Green"};
mcal::periph::DigitalOutput debug_do_red{"Debug: Red"};

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

void Log(std::string message) {
    std::cout << "(Log) " << message << std::endl;
}

}  // namespace bindings

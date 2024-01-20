/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <iostream>
#include <string>

namespace bindings {

mcal::periph::ADCInput temp_sensor_adc{"Temperature Sensor"};
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

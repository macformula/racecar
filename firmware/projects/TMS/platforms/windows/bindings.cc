/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.h"

#include <iostream>
#include <string>

namespace bindings {

mcal::periph::ADCInput temp_sensor_adc{1};
mcal::periph::PWMOutput fan_controller_pwm{1};

mcal::periph::DigitalOutput debug_do_green{1};
mcal::periph::DigitalOutput debug_do_red{2};

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

void Log(std::string message) {
    std::cout << "(Log) " << message << std::endl;
}

}  // namespace bindings

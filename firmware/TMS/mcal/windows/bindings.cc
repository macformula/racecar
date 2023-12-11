/// @author Blake Freer
/// @date 2023-12-10

#include <iostream>
#include <string>

#include "bindings.h"

namespace bindings {
	mcal::periph::ADCInput temp_sensor_adc(1);
	mcal::periph::PWMOutput fan_controller_pwm(1);
} // namespace bindings

void Initialize() {
	std::cout << "Initializing Windows System" << std::endl;
}

void Log(std::string message) {
	std::cout << "(Log) " << message << std::endl;
}

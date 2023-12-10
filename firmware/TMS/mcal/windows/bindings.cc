/// @author Blake Freer
/// @date 2023-12-10

#include <iostream>

#include "bindings.h"

namespace bindings {

	mcal::periph::ADCInput temp_sensor_adc(1);
	mcal::periph::PWMOutput fan_controller_pwm(1);

} // namespace bindings

void Initialize() {
	std::cout << "Initializing Windows System" << std::endl;
}

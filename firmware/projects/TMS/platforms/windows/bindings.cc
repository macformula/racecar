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

namespace bindings {

const shared::periph::ADCInput& temp_sensor_adc_1 =
    mcal::periph::ADCInput{"Temperature Sensor 1"};

const shared::periph::ADCInput& temp_sensor_adc_2 =
    mcal::periph::ADCInput{"Temperature Sensor 2"};

const shared::periph::ADCInput& temp_sensor_adc_3 =
    mcal::periph::ADCInput{"Temperature Sensor 3"};

const shared::periph::ADCInput& temp_sensor_adc_4 =
    mcal::periph::ADCInput{"Temperature Sensor 4"};

const shared::periph::ADCInput& temp_sensor_adc_5 =
    mcal::periph::ADCInput{"Temperature Sensor 5"};

const shared::periph::ADCInput& temp_sensor_adc_6 =
    mcal::periph::ADCInput{"Temperature Sensor 6"};

const shared::periph::PWMOutput& fan_controller_pwm =
    mcal::periph::PWMOutput{"Fan Controller"};

const shared::periph::DigitalOutput& debug_do_green =
    mcal::periph::DigitalOutput{"Debug: Green"};

const shared::periph::DigitalOutput& debug_do_red =
    mcal::periph::DigitalOutput{"Debug: Red"};

void Initialize() {
    std::cout << "Initializing Windows..." << std::endl;
}

void Log(std::string message) {
    std::cout << "(Log) " << message << std::endl;
}

}  // namespace bindings

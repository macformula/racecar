/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>
#include <cstring>

#include "app.h"
#include "bindings.h"

namespace bindings {
extern mcal::periph::PWMOutput fan_controller_pwm;
extern mcal::periph::ADCInput temp_sensor_adc;
}  // namespace bindings

// clang-format off
const float temp_lut_data[33][2] = {
    {2475, 120},
	{2480, 115},
	{2485, 110},
	{2491, 105},
	{2496, 100},
	{2501,  95},
	{2512,  90},
	{2517,  85},
	{2528,  80},
	{2543,  75},
	{2554,  70},
	{2570,  65},
	{2586,  60},
	{2607,  55},
	{2628,  50},
	{2649,  45},
	{2675,  40},
	{2707,  35},
	{2739,  30},
	{2771,  25},
	{2802,  20},
	{2839,  15},
	{2871,  10},
	{2903,   5},
	{2934,   0},
	{2966,  -5},
	{2987, -10},
	{3014, -15},
	{3029, -20},
	{3045, -25},
	{3056, -30},
	{3066, -35},
    {3077, -40},
};
// clang-format on

shared::util::LookupTable temp_lut{temp_lut_data, 33};

FanContoller fan_controller{bindings::fan_controller_pwm};
TempSensor temp_sensor{bindings::temp_sensor_adc, temp_lut};

int main(void) {
    Initialize();

    fan_controller.StartPWM();

    while (true) {
        float temperature = temp_sensor.Read();
        Log("Temperature: " + std::to_string(temperature));
        fan_controller.Set(temperature);
    }

    return 0;
}
/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"

#include "TMS/mcal/stm32f767/bindings.h"

namespace bindings {
    extern mcal::periph::PWMOutput fan_controller_pwm;
    extern mcal::periph::ADCInput temp_sensor_adc;
} // namespace bindings


const float const temp_lut_data[33][2] = {
    {3077, -40},
	{3066, -35},
	{3056, -30},
	{3045, -25},
	{3029, -20},
	{3014, -15},
	{2987, -10},
	{2966,  -5},
	{2934,   0},
	{2903,   5},
	{2871,  10},
	{2839,  15},
	{2802,  20},
	{2771,  25},
	{2739,  30},
	{2707,  35},
	{2675,  40},
	{2649,  45},
	{2628,  50},
	{2607,  55},
	{2586,  60},
	{2570,  65},
	{2554,  70},
	{2543,  75},
	{2528,  80},
	{2517,  85},
	{2512,  90},
	{2501,  95},
	{2496, 100},
	{2491, 105},
	{2485, 110},
	{2480, 115},
	{2475, 120},
};

shared::util::LookupTable temp_lut {temp_lut_data, 33};


FanContoller fan_controller{bindings::fan_controller_pwm};
TempSensor temp_sensor{bindings::temp_sensor_adc, temp_lut};

int main(void) {
    Initialize();

    fan_controller.StartPWM();

    while (true) {

    }

    return 0;
}
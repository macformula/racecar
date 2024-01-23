/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>
#include <string>

#include "app.h"
#include "bindings.h"

namespace bindings {
extern mcal::periph::ADCInput temp_sensor_adc_1;
extern mcal::periph::ADCInput temp_sensor_adc_2;
extern mcal::periph::ADCInput temp_sensor_adc_3;
extern mcal::periph::ADCInput temp_sensor_adc_4;
extern mcal::periph::ADCInput temp_sensor_adc_5;
extern mcal::periph::ADCInput temp_sensor_adc_6;

extern mcal::periph::PWMOutput fan_controller_pwm;

extern mcal::periph::DigitalOutput debug_do_green;
extern mcal::periph::DigitalOutput debug_do_red;

extern void Initialize();
extern void Log(std::string);
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

// clang-format off
const float fan_lut_data[3][2] = {
	{-1,    0},
	{ 0,   30},
	{50,  100}
};
// clang-format on

shared::util::LookupTable temp_adc_lut{temp_lut_data, 33};
shared::util::LookupTable fan_temp_lut{fan_lut_data, 3};

/***************************************************************
    Create app objects
***************************************************************/
FanContoller fan_controller{bindings::fan_controller_pwm, fan_temp_lut};
TempSensor temp_sensor_1{bindings::temp_sensor_adc_1, temp_adc_lut};
DebugIndicator debug_green{bindings::debug_do_green};
DebugIndicator debug_red{bindings::debug_do_red};

int main(void) {
    bindings::Initialize();

    fan_controller.StartPWM();

    while (true) {
        float temperature = temp_sensor_1.Read();
        bindings::Log("Temperature: " + std::to_string(temperature));
        fan_controller.Update(temperature);
    }

    return 0;
}
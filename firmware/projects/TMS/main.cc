/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>
#include <string>

#include "app.h"
#include "bindings.h"
#include "shared/util/algorithms/arrays.h"
#include "shared/util/mappers/lookup_table.h"

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

shared::util::LookupTable<33> temp_adc_lut{temp_lut_data};
shared::util::LookupTable<3> fan_temp_lut{fan_lut_data};

/***************************************************************
    Create app objects
***************************************************************/
FanContoller fan_controller{bindings::fan_controller_pwm, fan_temp_lut, 2.0f};

DebugIndicator debug_green{bindings::debug_do_green};
DebugIndicator debug_red{bindings::debug_do_red};

TempSensor temp_sensors[] = {
    TempSensor{bindings::temp_sensor_adc_1, temp_adc_lut},
    TempSensor{bindings::temp_sensor_adc_2, temp_adc_lut},
    TempSensor{bindings::temp_sensor_adc_3, temp_adc_lut},
    TempSensor{bindings::temp_sensor_adc_4, temp_adc_lut},
    TempSensor{bindings::temp_sensor_adc_5, temp_adc_lut},
    TempSensor{bindings::temp_sensor_adc_6, temp_adc_lut},
};

const int kSensorCount = 6;
TempSensorManager<kSensorCount> ts_manager{temp_sensors};

void UpdateTask() {
    static float temperature_buffer[kSensorCount];

    ts_manager.Update();
    ts_manager.GetTemperatures(temperature_buffer);

    float temp_min =
        shared::util::GetMinimum<float, kSensorCount>(temperature_buffer, NULL);

    float temp_max =
        shared::util::GetMaximum<float, kSensorCount>(temperature_buffer, NULL);

    float temp_avg =
        shared::util::GetAverage<float, kSensorCount>(temperature_buffer);

    /// TODO: Pack & send CAN message

    /// TODO: Needs PWM_Sweep_Nonblocking
    fan_controller.Update(temp_avg);
}

int main(void) {
    bindings::Initialize();

    fan_controller.StartPWM(0);

    while (true) {
    }

    return 0;
}
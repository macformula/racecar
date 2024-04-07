/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>
#include <string>

#include "app.h"
#include "shared/os/tick.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/algorithms/arrays.h"
#include "shared/util/mappers/lookup_table.h"

namespace bindings {
extern shared::periph::ADCInput& temp_sensor_adc_1;
extern shared::periph::ADCInput& temp_sensor_adc_2;
extern shared::periph::ADCInput& temp_sensor_adc_3;
extern shared::periph::ADCInput& temp_sensor_adc_4;
extern shared::periph::ADCInput& temp_sensor_adc_5;
extern shared::periph::ADCInput& temp_sensor_adc_6;

extern shared::periph::PWMOutput& fan_controller_pwm;

extern shared::periph::DigitalOutput& debug_do_blue;
extern shared::periph::DigitalOutput& debug_do_red;

extern void Initialize();
extern void Log(std::string);
}  // namespace bindings

namespace os {
extern void Tick(uint32_t ticks);
extern void InitializeKernel();
extern void StartKernel();
}  // namespace os

extern "C" {
void UpdateTask(void* argument);
}

const float temp_lut_data[][2] = {
    // clang-format off
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
    // clang-format on
};

const float fan_lut_data[][2] = {
    // clang-format off
	{-1,    0},
	{ 0,   30},
	{50,  100}
    // clang-format on
};

constexpr int temp_lut_length =
    (sizeof(temp_lut_data)) / (sizeof(temp_lut_data[0]));
shared::util::LookupTable<temp_lut_length> temp_adc_lut{temp_lut_data};

constexpr int fan_lut_length =
    (sizeof(fan_lut_data) / (sizeof(fan_lut_data[0])));
shared::util::LookupTable<fan_lut_length> fan_temp_lut{fan_lut_data};

/***************************************************************
    Create app objects
***************************************************************/
FanContoller fan_controller{bindings::fan_controller_pwm, fan_temp_lut, 2.0f};

DebugIndicator debug_blue{bindings::debug_do_blue};
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

/***************************************************************
    Program Logic
***************************************************************/

void Update() {
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

    fan_controller.Update(temp_avg);
}

void UpdateTask(void* argument) {
    const static uint32_t kTaskPeriodMs = 100;

    fan_controller.Start(0);
    while (true) {
        uint32_t start_time_ms = os::GetTickCount();
        Update();
        debug_blue.Toggle();  // toggling indicates the loop is running
        os::TickUntil(start_time_ms + kTaskPeriodMs);
    }
}

int main(void) {
    bindings::Initialize();
    os::InitializeKernel();

    os::StartKernel();

    while (true) continue;

    return 0;
}
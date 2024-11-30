/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/app.hpp"
#include "inc/tempsensor.hpp"
#include "shared/os/tick.hpp"
#include "shared/util/algorithms/arrays.hpp"
#include "shared/util/mappers/lookup_table.hpp"

using namespace generated::can;

namespace os {
extern void Tick(uint32_t ticks);
extern void InitializeKernel();
extern void StartKernel();
}  // namespace os

extern "C" {
void UpdateTask(void* argument);
}

/// Spin the fan faster when the acculumator is hotter.
const float fan_lut_data[][2] = {
    {19, 0},
    {20, 30},
    {50, 100},
};

constexpr int fan_lut_length =
    (sizeof(fan_lut_data) / (sizeof(fan_lut_data[0])));
shared::util::LookupTable<fan_lut_length> fan_temp_lut{fan_lut_data};

/***************************************************************
    Create CAN objects
***************************************************************/

VehBus veh_can_bus{bindings::veh_can_base};

/***************************************************************
    Create app objects
***************************************************************/
FanContoller fan_controller{bindings::fan_controller_pwm, fan_temp_lut, 2.0f};

TempSensor temp_sensors[] = {
    TempSensor{bindings::temp_sensor_adc_1, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_2, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_3, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_4, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_5, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_6, tempsensor::volt_stm_to_degC},
};
constexpr int kSensorCount = sizeof(temp_sensors) / sizeof(temp_sensors[0]);

BmsBroadcaster bms_broadcaster(veh_can_bus, kSensorCount);

/***************************************************************
    Program Logic
***************************************************************/

void Update() {
    float temperatures[kSensorCount];
    for (int i = 0; i < kSensorCount; i++) {
        temperatures[i] = temp_sensors[i].Update();
    }

    uint8_t low_thermistor_idx;
    float temp_min = shared::util::GetMinimum<float, kSensorCount>(
        temperatures, &low_thermistor_idx);

    uint8_t high_thermistor_idx;
    float temp_max = shared::util::GetMaximum<float, kSensorCount>(
        temperatures, &high_thermistor_idx);

    float temp_avg =
        shared::util::GetAverage<float, kSensorCount>(temperatures);

    bms_broadcaster.SendBmsBroadcast(
        high_thermistor_idx, static_cast<int8_t>(temp_max), low_thermistor_idx,
        static_cast<int8_t>(temp_min), static_cast<int8_t>(temp_avg));

    fan_controller.Update(temp_avg);

    // Toggle the green LED to indicate the program is running
    static bool debug_green = false;
    bindings::debug_led_green.Set(debug_green);
    debug_green = !debug_green;
}

void UpdateTask(void* argument) {
    (void)argument;  // prevent unused variable warning
    const static uint32_t kTaskPeriodMs = 100;

    fan_controller.Start(0);
    while (true) {
        uint32_t start_time_ms = os::GetTickCount();
        Update();
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
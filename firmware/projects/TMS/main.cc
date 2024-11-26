/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>
#include <string>

#include "bindings.h"
#include "generated/can/veh_can_messages.hpp"
#include "generated/can/veh_msg_registry.hpp"
#include "inc/app.h"
#include "shared/comms/can/can_bus.hpp"
#include "shared/os/tick.hpp"
#include "shared/periph/adc.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/algorithms/arrays.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/lookup_table.hpp"
#include "shared/util/mappers/mapper.hpp"

namespace os {
extern void Tick(uint32_t ticks);
extern void InitializeKernel();
extern void StartKernel();
}  // namespace os

extern "C" {
void UpdateTask(void* argument);
}

namespace tempsensor {

/// This table is directly copied from Table 4 of the temperature sensor
/// datasheet. `datasheets/energus/Datasheet_with_VTC6_rev_A(2021-10-26).pdf`
const float ts_table[][2] = {
    // clang-format off
    {1.30f, 120.0f},
    {1.31f, 115.0f},
    {1.32f, 110.0f},
    {1.33f, 105.0f},
    {1.34f, 100.0f},
    {1.35f,  95.0f},
    {1.37f,  90.0f},
    {1.38f,  85.0f},
    {1.40f,  80.0f},
    {1.43f,  75.0f},
    {1.45f,  70.0f},
    {1.48f,  65.0f},
    {1.51f,  60.0f},
    {1.55f,  55.0f},
    {1.59f,  50.0f},
    {1.63f,  45.0f},
    {1.68f,  40.0f},
    {1.74f,  35.0f},
    {1.80f,  30.0f},
    {1.86f,  25.0f},
    {1.92f,  20.0f},
    {1.99f,  15.0f},
    {2.05f,  10.0f},
    {2.11f,   5.0f},
    {2.17f,   0.0f},
    {2.23f,  -5.0f},
    {2.27f, -10.0f},
    {2.32f, -15.0f},
    {2.35f, -20.0f},
    {2.38f, -25.0f},
    {2.40f, -30.0f},
    {2.42f, -35.0f},
    {2.44f, -40.0f}
    // clang-format on
};
constexpr int lut_length = (sizeof(ts_table)) / (sizeof(ts_table[0]));
const shared::util::LookupTable<lut_length> volt_ts_to_degC{ts_table};

/// Calculate the voltage at the temperature sensor from the voltage at the STM.
/// They are not equal because there is a non-unity gain buffer between them.
/// V_STM = 1.44 + 0.836 * V_TS / 2
/// So the inverse is
/// V_TS = 2 * (V_STM - 1.44) / 0.836 = 2/0.836 * V_STM - 2*1.44/0.836
const shared::util::LinearMap<float> volt_stm_to_volt_ts{
    2.0f / 0.836f,
    -2.0f * 1.44f / 0.836f,
};

/// Compose the two maps to get the final map from the STM voltage to the
/// temperature in degrees C.
const shared::util::CompositeMap<float> volt_stm_to_degC{
    volt_ts_to_degC,      // outer (second) function
    volt_stm_to_volt_ts,  // inner (first) function
};

}  // namespace tempsensor

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

generated::can::VehMsgRegistry veh_can_registry{};

shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
};

/***************************************************************
    Create app objects
***************************************************************/
FanContoller fan_controller{bindings::fan_controller_pwm, fan_temp_lut, 2.0f};

DebugIndicator debug_green{bindings::debug_led_green};
DebugIndicator debug_red{bindings::debug_led_red};

TempSensor temp_sensors[] = {
    TempSensor{bindings::temp_sensor_adc_1, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_2, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_3, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_4, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_5, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_6, tempsensor::volt_stm_to_degC},
};

const int kSensorCount = 6;
TempSensorManager<kSensorCount> ts_manager{temp_sensors};

BmsBroadcaster bms_broadcaster(veh_can_bus, kSensorCount);

/***************************************************************
    Program Logic
***************************************************************/

void Update() {
    static float temperature_buffer[kSensorCount];
    static uint8_t low_thermistor_idx;
    static uint8_t high_thermistor_idx;

    debug_green.Toggle();

    veh_can_bus.Update();
    ts_manager.Update();
    ts_manager.GetTemperatures(temperature_buffer);

    float temp_min = shared::util::GetMinimum<float, kSensorCount>(
        temperature_buffer, &low_thermistor_idx);

    float temp_max = shared::util::GetMaximum<float, kSensorCount>(
        temperature_buffer, &high_thermistor_idx);

    float temp_avg =
        shared::util::GetAverage<float, kSensorCount>(temperature_buffer);

    bms_broadcaster.SendBmsBroadcast(
        high_thermistor_idx, static_cast<int8_t>(temp_max), low_thermistor_idx,
        static_cast<int8_t>(temp_min), static_cast<int8_t>(temp_avg));

    fan_controller.Update(temp_avg);
}

void UpdateTask(void* argument) {
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
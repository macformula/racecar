/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/app.hpp"
#include "inc/tempsensor.hpp"
#include "shared/util/algorithms/arrays.hpp"
#include "shared/util/mappers/lookup_table.hpp"

using namespace generated::can;

TempSensor temp_sensors[] = {
    TempSensor{bindings::temp_sensor_adc_1, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_2, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_3, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_4, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_5, tempsensor::volt_stm_to_degC},
    TempSensor{bindings::temp_sensor_adc_6, tempsensor::volt_stm_to_degC},
};
constexpr int kSensorCount = sizeof(temp_sensors) / sizeof(temp_sensors[0]);

/// Spin the fan faster when the acculumator is hotter.
const float fan_lut_data[][2] = {
    {19, 0},
    {20, 30},
    {50, 100},
};

constexpr int fan_lut_length =
    (sizeof(fan_lut_data) / (sizeof(fan_lut_data[0])));
shared::util::LookupTable<fan_lut_length> fan_temp_lut{fan_lut_data};

FanContoller fan_controller{bindings::fan_controller_pwm, fan_temp_lut};

VehBus veh_can_bus{bindings::veh_can_base};

TxBmsBroadcast PackBmsBroadcast(float temperatures[]) {
    // This is a constant defined by Orion. It was discovered by
    // decoding the CAN traffic coming from the Orion Thermal Expansion
    // Pack.
    const uint8_t kBmsChecksumConstant = 0x41;
    const uint8_t kThermistorModuleNumber = 0;

    uint8_t low_index;
    int8_t low_temp =
        static_cast<int8_t>(shared::util::GetMinimum<float, kSensorCount>(
            temperatures, &low_index));

    uint8_t high_index;
    int8_t high_temp =
        static_cast<int8_t>(shared::util::GetMaximum<float, kSensorCount>(
            temperatures, &high_index));

    int8_t temp_avg = static_cast<int8_t>(
        shared::util::GetAverage<float, kSensorCount>(temperatures));

    uint8_t checksum = kThermistorModuleNumber + low_temp + high_temp +
                       temp_avg + kSensorCount + high_index + low_index +
                       kBmsChecksumConstant;

    return TxBmsBroadcast{
        .therm_module_num = kThermistorModuleNumber,
        .low_therm_value = low_temp,
        .high_therm_value = high_temp,
        .avg_therm_value = temp_avg,
        .num_therm_en = kSensorCount,
        .high_therm_id = high_index,
        .low_therm_id = low_index,
        .checksum = checksum,
    };
}

/***************************************************************
    Program Logic
***************************************************************/
void Update(float update_period_ms) {
    // Read the temperature sensors
    float temperatures[kSensorCount];
    for (int i = 0; i < kSensorCount; i++) {
        temperatures[i] = temp_sensors[i].Update();
    }

    // Send the temperatures to the BMS
    TxBmsBroadcast bms_broadcast = PackBmsBroadcast(temperatures);
    veh_can_bus.Send(bms_broadcast);

    // Adjust the fan speed based on the average temperature
    float avg_temperature =
        shared::util::GetAverage<float, kSensorCount>(temperatures);
    fan_controller.Update(avg_temperature, update_period_ms);

    // Toggle the green LED to indicate the program is running
    static bool toggle = true;
    bindings::debug_led_green.Set(toggle);
    toggle = !toggle;
}

int main(void) {
    bindings::Initialize();

    const uint32_t kUpdatePeriodMs = 100;

    while (true) {
        uint32_t start_time_ms = bindings::GetCurrentTimeMs();
        Update(kUpdatePeriodMs);
        while (bindings::GetCurrentTimeMs() <= start_time_ms + kUpdatePeriodMs);
    }

    return 0;
}
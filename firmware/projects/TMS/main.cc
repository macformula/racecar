/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "bindings.hpp"
#include "etl/array.h"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/fan_controller.hpp"
#include "inc/temp_sensor.hpp"
#include "shared/os/tick.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/util/arrays.hpp"

using namespace generated::can;

etl::array temp_sensors{
    TempSensor{bindings::temp_sensor_adc_1},
    TempSensor{bindings::temp_sensor_adc_2},
    TempSensor{bindings::temp_sensor_adc_3},
    TempSensor{bindings::temp_sensor_adc_4},
    TempSensor{bindings::temp_sensor_adc_5},
    TempSensor{bindings::temp_sensor_adc_6},
};
constexpr int kSensorCount = temp_sensors.size();

FanContoller fan_controller{bindings::fan_controller_pwm};

VehBus veh_can_bus{bindings::veh_can_base};

TxBmsBroadcast PackBmsBroadcast(float temperatures[]) {
    // This is a constant defined by Orion. It was discovered by
    // decoding the CAN traffic coming from the Orion Thermal Expansion Pack.
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
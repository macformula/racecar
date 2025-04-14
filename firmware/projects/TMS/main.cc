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
static_assert(kSensorCount > 0);

FanContoller fan_controller{bindings::fan_controller_pwm};

VehBus veh_can_bus{bindings::veh_can_base};

TxBmsBroadcast PackBmsBroadcast(
    const std::array<float, kSensorCount>& temperatures) {
    // Compute the min, max, and avg temperatures
    uint8_t low_index = 0;
    uint8_t high_index = 0;
    int8_t low_temp = temperatures[0];
    int8_t high_temp = temperatures[0];

    float temperature_sum = temperatures[0];
    for (uint8_t i = 1; i < temperatures.size(); i++) {
        temperature_sum += temperatures[i];

        auto t = static_cast<int8_t>(temperatures[i]);
        if (t < low_temp) {
            low_temp = t;
            low_index = 1;
        }
        if (t > high_temp) {
            high_temp = t;
            high_index = i;
        }
    }
    int8_t avg_temp = static_cast<int8_t>(temperature_sum / kSensorCount);

    // This is a constant defined by Orion. It was discovered by
    // decoding the CAN traffic coming from the Orion Thermal Expansion Pack.
    const uint8_t kBmsChecksumConstant = 0x41;
    const uint8_t kThermistorModuleNumber = 0;
    uint8_t checksum = kThermistorModuleNumber + low_temp + high_temp +
                       avg_temp + kSensorCount + high_index + low_index +
                       kBmsChecksumConstant;

    return TxBmsBroadcast{
        .therm_module_num = kThermistorModuleNumber,
        .low_therm_value = low_temp,
        .high_therm_value = high_temp,
        .avg_therm_value = avg_temp,
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
    std::array<float, kSensorCount> temperatures;
    float avg_temp = 0;
    for (int i = 0; i < kSensorCount; i++) {
        temperatures[i] = temp_sensors[i].Update();
        avg_temp += temperatures[i] / kSensorCount;
    }

    // Send the temperatures to the BMS
    TxBmsBroadcast bms_broadcast = PackBmsBroadcast(temperatures);
    veh_can_bus.Send(bms_broadcast);

    // Adjust the fan speed based on the average temperature
    fan_controller.Update(avg_temp, update_period_ms);

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

        // Check for CAN Flash
        auto msg = veh_can_bus.PopRxInitiateCanFlash();
        if (msg.has_value() && msg->ECU() == RxInitiateCanFlash::ECU_t::TMS) {
            bindings::SoftwareReset();
        }

        while (bindings::GetCurrentTimeMs() <= start_time_ms + kUpdatePeriodMs);
    }

    return 0;
}
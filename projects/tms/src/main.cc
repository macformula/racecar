/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "../../../include/generated/githash.hpp"
#include "bindings.hpp"
#include "etl/array.h"
#include "fan_controller/fan_controller.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "periph/gpio.hpp"
#include "temp_sensor/temp_sensor.hpp"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

static const size_t STACK_SIZE_WORDS = 2048 * 16;
static const uint32_t PRIORITY_10HZ = 2;
static const uint32_t PRIORITY_1HZ = 1;

StaticTask_t t10hz_control_block;
StackType_t t10hz_buffer[STACK_SIZE_WORDS];

StaticTask_t t1hz_control_block;
StackType_t t1hz_buffer[STACK_SIZE_WORDS];

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

FanController fan_controller{bindings::fan_controller_pwm};

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
    bindings::debug_led_red.Set(toggle);
    toggle = !toggle;
}

void task_1hz(void* argument) {
    (void)argument;

    const uint32_t kUpdatePeriodMs = 1000;
    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        veh_can_bus.Send(TxTmsGitHash{
            .commit = macfe::generated::GIT_HASH,
            .dirty = macfe::generated::GIT_DIRTY,
        });

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(kUpdatePeriodMs));
    }
}

void task_10hz(void* argument) {
    (void)argument;

    const uint32_t kUpdatePeriodMs = 100;
    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        Update(kUpdatePeriodMs);

        // Check for CAN Flash
        auto msg = veh_can_bus.PopRxInitiateCanFlash();
        if (msg.has_value() && msg->ECU() == RxInitiateCanFlash::ECU_t::TMS) {
            bindings::SoftwareReset();
        }

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(kUpdatePeriodMs));
    }
}

int main(void) {
    bindings::Initialize();

    xTaskCreateStatic(task_10hz, "10HZ", STACK_SIZE_WORDS, NULL, PRIORITY_10HZ,
                      t10hz_buffer, &t10hz_control_block);

    xTaskCreateStatic(task_1hz, "1HZ", STACK_SIZE_WORDS, NULL, PRIORITY_1HZ,
                      t1hz_buffer, &t1hz_control_block);

    vTaskStartScheduler();

    while (true) continue;

    return 0;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              char* pcTaskName);

// Optional: call this periodically, e.g., from task_10hz

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              char* pcTaskName) {
    // Handle stack overflow (e.g., light an LED, halt, etc.)
    bindings::debug_led_red.SetHigh();
    macfe::periph::CanErrorHandler(&bindings::veh_can_base);
    while (1) {
        // Trap CPU here
    }
}

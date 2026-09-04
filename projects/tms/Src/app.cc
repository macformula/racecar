/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

// CubeMX / STM32 HAL
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// The HAL defines `CRC` as an object-like macro for the CRC peripheral, which
// mangles the CRC() accessor on generated CAN messages. TMS doesn't use that
// peripheral. This must stay between the HAL headers and the generated ones -
// the blank-line-separated blocks keep clang-format from re-sorting across it.
#undef CRC

#include "etl/array.h"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "generated/githash.hpp"
#include "mcal/stm32f/analog_input.hpp"
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "mcal/stm32f/pwm.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"
#include "tms-common/fan_controller.hpp"
#include "tms-common/temp_sensor.hpp"
#include "tms-common/tms-common.hpp"

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

using namespace mcal::stm32f;

AnalogInput temp_sensor_adc_1{&hadc1, SENS_1_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_2{&hadc1, SENS_2_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_3{&hadc1, SENS_3_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_4{&hadc1, SENS_4_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_5{&hadc1, SENS_5_UC_IN_CHANNEL};
AnalogInput temp_sensor_adc_6{&hadc1, SENS_6_UC_IN_CHANNEL};

PWMOutput fan_controller_pwm{&htim4, TIM_CHANNEL_1};
DigitalOutput debug_led_green{DEBUG_LED1_GPIO_Port, DEBUG_LED1_Pin};
DigitalOutput debug_led_red{DEBUG_LED2_GPIO_Port, DEBUG_LED2_Pin};
DigitalOutput debug_led_nucleo{NUCLEO_RED_LED_GPIO_Port, NUCLEO_RED_LED_Pin};

CanBase veh_can_base{&hcan2};

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined in
 * cubemx/../main.c
 */
void SystemClock_Config();
}

void Initialize() {
    HAL_Init();
    SystemClock_Config();
    MX_ADC1_Init();
    MX_TIM4_Init();
    MX_GPIO_Init();
    MX_CAN2_Init();

    veh_can_base.Setup();
}

uint32_t GetCurrentTimeMs() {
    return HAL_GetTick();
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

using namespace generated::can;

etl::array temp_sensors{
    TempSensor{temp_sensor_adc_1}, TempSensor{temp_sensor_adc_2},
    TempSensor{temp_sensor_adc_3}, TempSensor{temp_sensor_adc_4},
    TempSensor{temp_sensor_adc_5}, TempSensor{temp_sensor_adc_6},
};

FanController fan_controller{fan_controller_pwm};

VehBus veh_can_bus{veh_can_base};

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
        macfe::tms::ReadAndSendTemperatures(kUpdatePeriodMs, veh_can_bus,
                                            fan_controller, temp_sensors);

        // Toggle the green LED to indicate the program is running
        static bool toggle = true;
        debug_led_green.Set(toggle);
        debug_led_red.Set(toggle);
        toggle = !toggle;

        // Check for CAN Flash
        auto msg = veh_can_bus.PopRxInitiateCanFlash();
        if (msg.has_value() && msg->ECU() == RxInitiateCanFlash::ECU_t::TMS) {
            SoftwareReset();
        }

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(kUpdatePeriodMs));
    }
}

int main(void) {
    Initialize();

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
    debug_led_red.SetHigh();
    macfe::periph::CanErrorHandler(&veh_can_base);
    while (1) {
        // Trap CPU here
    }
}

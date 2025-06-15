/// The Demo runs two FreeRTOS tasks in parallel. Designed for a NUCLEO-F767
/// board.
/// 1. LedTask: Toggles the red LED at regular intervals
/// 2. ButtonTask: Controls the blue led with the button.

#include "bindings.hpp"

// Set up FreeRTOS objects. Everything is statically allocated which means we
// must explicitly define the memory. Dynamic allocation would require less
// code, but it is less predictable and should not be used in embeded systems.

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

// Tasks
// How much memory does each task get to use?
static const size_t STACK_NUM_WORDS = 256;

TaskHandle_t led_task;  // reference to the task
StackType_t led_task_stack_allocation[STACK_NUM_WORDS];  // memory allocation
StaticTask_t led_task_control_block;  // task management storage

TaskHandle_t button_task;
StackType_t button_task_stack_allocation[STACK_NUM_WORDS];
StaticTask_t button_task_control_block;

// Task Priorities. High number = higher priority
// More frequent tasks should generally have a higher priority
uint32_t TASK_PRIORITY_BUTTON = 2;
uint32_t TASK_PRIORITY_LED = 1;

// Define the task functions.

// Blink the red LED at 5Hz (10Hz toggle = 5Hz blink)
void LedTask(void* argument) {
    // We don't need the argument. This silences the compiler warning.
    (void)argument;

    // Remembers the last time this task loop ran
    TickType_t last_wake_time = xTaskGetTickCount();

    bool toggle_value = true;
    while (true) {
        // Toggle the LED
        bindings::red_led.Set(toggle_value);
        toggle_value = !toggle_value;

        // Pause the loop until 100ms have elapsed since the last run.
        // This is better than a regular delay since it ensures the loop runs
        // onces every 100ms, regardless of how long the logic takes.
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(100));
    }
}

// Control the blue LED with the button
void ButtonTask(void* argument) {
    (void)argument;

    TickType_t last_wake_time;

    while (true) {
        bool button_pressed = bindings::button.Read();
        bindings::blue_led.Set(button_pressed);

        // Delays are measured in ticks, but we usually think in milliseconds
        // Fortunately, FreeRTOS defines a macro to convert between them.
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1));
    }
}

int main(void) {
    bindings::Initialize();

    // Create the tasks. They will NOT start running yet
    button_task = xTaskCreateStatic(
        ButtonTask, "button task", STACK_NUM_WORDS, NULL, TASK_PRIORITY_BUTTON,
        button_task_stack_allocation, &button_task_control_block);

    led_task = xTaskCreateStatic(LedTask, "led task", STACK_NUM_WORDS, NULL,
                                 TASK_PRIORITY_LED, led_task_stack_allocation,
                                 &led_task_control_block);

    // Run the scheduler
    vTaskStartScheduler();

    // the program will never reach this line
    while (true) continue;
    return 0;
}

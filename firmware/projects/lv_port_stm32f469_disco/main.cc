#include "main.h"

#include "bindings.hpp"
#include "gpio.h"
#include "lvgl.h"

extern "C" {
#include "screen_driver.h"
#include "touch_sensor_driver.h"
}

extern "C" {
extern lv_disp_drv_t lv_display_driver;
}
int main(void) {
    bindings::Initialize();

    lv_init();

    screen_driver_init();
    touch_sensor_driver_init();

    // lv_demo_benchmark();
    lv_obj_t* label1 = lv_label_create(lv_scr_act());

    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    char text[2] = {'*', '\0'};
    lv_label_set_text(label1, text);
    lv_obj_center(label1);

    uint8_t counter = 0;
    int led = true;

    while (1) {
        lv_timer_handler();
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,
                          static_cast<GPIO_PinState>(led));
        led = !led;

        text[0] = counter + 48;
        counter++;
        if (counter >= 10) counter = 0;
        lv_label_set_text(label1, text);

        bindings::DelayMs(50);
    }
}

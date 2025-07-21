#include <unistd.h>

#include <iostream>
#include <unordered_map>

#include "mcal/linux/can.hpp"
#include "shared/periph/gpio.hpp"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" \
                            issue*/

#include <immintrin.h>
extern "C" {
#include <SDL2/SDL.h>

#include "lv_drivers/display/monitor.h"
#include "lvgl.h"

lv_disp_drv_t lv_display_driver;
}

void hal_init(void);

/// Use SDL inputs to determine if keys are pressed or not
/// Better than the mcal linux DigitalInput for project since it is non-blocking
class KeyboardInput : public shared::periph::DigitalInput {
public:
    KeyboardInput(SDL_Keycode key) : key_(key) {}

    bool Read() override {
        PollEvents();
        return keyStates_[key_];
    }

    static void PollEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                keyStates_[event.key.keysym.sym] = true;
            } else if (event.type == SDL_KEYUP) {
                keyStates_[event.key.keysym.sym] = false;
            }
        }
    }

private:
    SDL_Keycode key_;
    static inline std::unordered_map<SDL_Keycode, bool> keyStates_;
};

namespace mcal {
using namespace mcal::lnx;

CanBase veh_can_base{"vcan0"};
KeyboardInput button_scroll{SDLK_TAB};
KeyboardInput button_select{SDLK_SPACE};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::DigitalInput& button_scroll = mcal::button_scroll;
shared::periph::DigitalInput& button_enter = mcal::button_select;

int tick_thread(void* _data) {
    // This is only needed because the Linux platform doesn't support
    // LV_TICK_CUSTOM like STM does.
    (void)_data;

    while (true) {
        lv_tick_inc(5);
        SDL_Delay(5);
    }
    return 0;
}

void Initialize() {
    std::cout << "Starting Linux Platform" << std::endl;
    mcal::veh_can_base.Setup();

    // LVGL Init
    monitor_init();

    /* Create a display buffer */
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
    static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

    /* Create a display */
    lv_disp_drv_init(&lv_display_driver);
    lv_display_driver.draw_buf = &disp_buf1;
    lv_display_driver.flush_cb = monitor_flush;
    lv_display_driver.hor_res = 800;
    lv_display_driver.ver_res = 480;
    lv_display_driver.antialiasing = 1;

    lv_disp_t* disp = lv_disp_drv_register(&lv_display_driver);

    SDL_CreateThread(tick_thread, "tick", NULL);
}

void DelayMS(uint32_t ms) {
    SDL_Delay(ms);
}

}  // namespace bindings

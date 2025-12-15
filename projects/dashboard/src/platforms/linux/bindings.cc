#include <unistd.h>

#include <csignal>
#include <iostream>
#include <unordered_map>

#include "mcal/linux/can.hpp"
#include "periph/gpio.hpp"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" \
                            issue*/

#include <immintrin.h>
extern "C" {
#include <SDL2/SDL.h>

#include "lvgl.h"

lv_display_t* lv_display;
lv_indev_t* kb_indev;
}

void hal_init(void);

// Signal handler for Ctrl+C
static volatile bool signal_received = false;
void signal_handler(int signum) {
    (void)signum;
    signal_received = true;
    std::cout << "\nCtrl+C detected, shutting down...\n" << std::flush;
}

/// Use SDL inputs to determine if keys are pressed or not
/// Better than the mcal linux DigitalInput for project since it is non-blocking
class KeyboardInput : public macfe::periph::DigitalInput {
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
            } else if (event.type == SDL_QUIT) {
                quit_requested_ = true;
            }
        }
    }

    static bool IsQuitRequested() {
        return quit_requested_ || signal_received;
    }

private:
    SDL_Keycode key_;
    static inline std::unordered_map<SDL_Keycode, bool> keyStates_;
    static inline bool quit_requested_ = false;
};

namespace mcal {
using namespace mcal::lnx;

CanBase veh_can_base{"vcan0"};
KeyboardInput button_scroll{SDLK_TAB};
KeyboardInput button_select{SDLK_SPACE};

}  // namespace mcal

namespace bindings {

macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;
macfe::periph::DigitalInput& button_scroll = mcal::button_scroll;
macfe::periph::DigitalInput& button_enter = mcal::button_select;

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

    // Set up signal handlers for clean shutdown
    std::signal(SIGINT, signal_handler);   // Ctrl+C
    std::signal(SIGTERM, signal_handler);  // Termination signal

    mcal::veh_can_base.Setup();

    /* Create SDL window with LVGL 9.3 native driver */
    lv_display = lv_sdl_window_create(800, 480);

    /* Create tick thread */
    SDL_CreateThread(tick_thread, "tick", NULL);
}

void Shutdown() {
    std::cout << "Shutting down dashboard..." << std::endl;
    std::cout << "Cleaning up CAN and SDL resources..." << std::endl;
    // CAN cleanup happens automatically via destructors
    SDL_Quit();

    // Force immediate exit to avoid any remaining cleanup issues
    // This is called after lv_deinit() in main()
    _exit(0);
}

void DelayMS(uint32_t ms) {
    SDL_Delay(ms);
}

bool ShouldQuit() {
    return KeyboardInput::IsQuitRequested();
}

}  // namespace bindings

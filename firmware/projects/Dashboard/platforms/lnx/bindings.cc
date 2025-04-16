#include <unistd.h>

#include <iostream>
#include <unordered_map>

#include "mcal/linux/periph/can.hpp"
#include "mcal/linux/periph/digital_input.hpp"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" \
                            issue*/

#include <immintrin.h>
extern "C" {
#include <SDL2/SDL.h>
}

void hal_init(void);

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
using namespace mcal::lnx::periph;

CanBase veh_can_base{"vcan0"};
KeyboardInput button_scroll{SDLK_TAB};
KeyboardInput button_select{SDLK_SPACE};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::DigitalInput& button_scroll = mcal::button_scroll;
shared::periph::DigitalInput& button_select = mcal::button_select;

void Initialize() {
    std::cout << "Starting Linux Platform" << std::endl;
    mcal::veh_can_base.Setup();
}

void PostLvglInit() {
    hal_init();
}

void DelayMS(uint32_t ms) {
    usleep(ms * 1000);
}
}  // namespace bindings

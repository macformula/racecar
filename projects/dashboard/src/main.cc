#include "Button.hpp"
#include "Display.hpp"
#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "lvgl.h"

#include <iostream>

extern "C" {
extern lv_display_t* lv_display;
}

using namespace generated::can;

Button btn_enter{bindings::button_enter};
Button btn_scroll{bindings::button_scroll};
VehBus veh_can{bindings::veh_can_base};

Display display{btn_enter, btn_scroll, veh_can};

const int kUpdatePeriodMs = 20;
static uint8_t tx_counter = 0;

int main(void) {
    lv_init();
    bindings::Initialize();

    display.Start();

    while (!bindings::ShouldQuit()) {
        try {
            int time_ms = lv_tick_get();

            display.Update(time_ms);

            veh_can.Send(TxDashStatus{
                .counter = tx_counter++,
                .state = display.GetState(),
                .profile = display.selected_profile,
            });

            bindings::DelayMS(kUpdatePeriodMs);
        } catch (const std::exception& e) {
            std::cerr << "Exception in main loop: " << e.what() << std::endl;
            break;
        } catch (...) {
            std::cerr << "Unknown exception in main loop" << std::endl;
            break;
        }
    }
    
    std::cout << "Shutting down dashboard..." << std::endl;
    
    // Clean shutdown
    bindings::Shutdown();
    lv_deinit();
    
    // Force immediate exit to avoid any remaining cleanup issues
    _exit(0);
}

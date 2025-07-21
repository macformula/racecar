#include "Button.hpp"
#include "Display.hpp"
#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "lvgl.h"

extern "C" {
extern lv_disp_drv_t lv_display_driver;
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

    while (true) {
        int time_ms = lv_tick_get();

        display.Update(time_ms);

        veh_can.Send(TxDashStatus{
            .counter = tx_counter++,
            .state = display.GetState(),
            .profile = display.selected_profile,
        });

        bindings::DelayMS(kUpdatePeriodMs);
    }
}

#pragma once

#include "Screen.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "lvgl/lvgl.h"

class EventSelect : public Screen {
public:
    using Event = generated::can::TxDashboardStatus::Event_t;

    EventSelect(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;

private:
    const uint8_t kEventCount = static_cast<uint8_t>(Event::UNSPECIFIED);
    // create the roller object that stores the modes names
    lv_obj_t* roller;
};

const char* GetEventName(EventSelect::Event e);
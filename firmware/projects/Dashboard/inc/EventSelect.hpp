#pragma once

#include "Screen.hpp"
#include "generated/can/veh_messages.hpp"
#include "lvgl.h"

class EventSelect : public Screen {
public:
    using Event = generated::can::TxDashboardStatus::Event_t;

    EventSelect(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    const uint8_t kEventCount = static_cast<uint8_t>(Event::UNSPECIFIED);
    lv_obj_t* roller;
};

const char* GetEventName(EventSelect::Event e);
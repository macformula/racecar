#pragma once

#include "Screen.hpp"
#include "generated/can/veh_messages.hpp"
#include "lvgl.h"

class ProfileSelect : public Screen {
public:
    using Profile = generated::can::TxDashboardStatus::Profile_t;

    ProfileSelect(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    const uint8_t kEventCount = static_cast<uint8_t>(Profile::_ENUM_TAIL_);
    lv_obj_t* roller;
};

const char* GetProfileName(ProfileSelect::Profile e);
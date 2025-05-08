#pragma once

#include "Screen.hpp"
#include "generated/can/veh_messages.hpp"

class DriverSelect : public Screen {
public:
    using Driver = generated::can::TxDashboardStatus::Driver_t;

    DriverSelect(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    const uint8_t kNumDrivers = static_cast<uint8_t>(Driver::UNSPECIFIED);
    lv_obj_t* roller;
};

const char* GetDriverName(DriverSelect::Driver d);

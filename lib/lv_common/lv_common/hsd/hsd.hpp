/// @author Manush Patel
/// @date 2026-05-23

#pragma once
#include <cstdint>

#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace hsd {

using macfe::periph::AnalogInput;
using macfe::periph::DigitalOutput;

struct hsd {
    macfe::periph::AnalogInput* isense;
    macfe::periph::DigitalOutput* isense_en;
    macfe::periph::DigitalOutput** sel;
    int num_sel;
    float* prev_tick_ch;
};
struct hsd_periph {
    hsd hsd[];
};
struct Reading {
    float current_ma = 0.0f;
    bool fault = false;
};
class Controller {
public:
    Controller(hsd_periph hsd_periph) {
        for (int i = 0; i < sizeof(hsd_periph); i++) {
            size_t sel_size = sizeof(hsd_periph.hsd[i].sel);
            hsd_periph.hsd[i].num_sel = sel_size;
            hsd_periph.hsd[i].prev_tick_ch = new float[2 << (sel_size - 1)];
            channel_num += (2 << (sel_size - 1));
        }
        periph = hsd_periph;
        channels = new Reading[channel_num];
    };
    void Update_10Hz(generated::can::VehBus& veh_can);

private:
    hsd_periph periph;
    Reading Read(uint8_t hsd, uint8_t channel);

    bool HasOverCurrent();

    float kVoltsToMa_4ch = 1500.0f / 0.535f;    // HSD_1
    float kVoltsToMa_2ch = 10000.0f / 0.4815f;  // HSD_2–6
    float kFaultThresholdV = 0.64f;
    Reading* channels;
    int channel_num = 0;
};

}  // namespace hsd

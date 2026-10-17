/// @author Manush Patel
/// @date 2026-05-23

#pragma once
#include <stdint.h>

#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace macfe::fc {
using macfe::periph::AnalogInput;
using macfe::periph::DigitalOutput;

struct Reading {
    float current_ma = 0.0f;
    bool fault = false;
};

class HSD_Base {
public:
    virtual Reading Read(uint8_t channel) = 0;
};

class HSD1 : public HSD_Base {
public:
    HSD1(AnalogInput& isense, DigitalOutput& isense_en)
        : _isense(isense), _isense_en(isense_en) {};
    Reading Read(uint8_t channel) override;

private:
    AnalogInput& _isense;
    DigitalOutput& _isense_en;
    float prev_tick = 0;
};

class HSD4 : public HSD_Base {
public:
    HSD4(AnalogInput& isense, DigitalOutput& isense_en, DigitalOutput& sel1,
         DigitalOutput& sel2)
        : _isense(isense), _isense_en(isense_en), _sel1(sel1), _sel2(sel2) {};
    Reading Read(uint8_t channel) override;

private:
    AnalogInput& _isense;
    DigitalOutput& _isense_en;
    DigitalOutput& _sel1;
    DigitalOutput& _sel2;
    float prev_tick_ch[4] = {};
};

class HSD {
public:
    HSD(HSD_Base& hsd1, HSD_Base& hsd2) : _hsds{&hsd1, &hsd2} {}

    void Update_10Hz(generated::can::VehBus& bus);

    bool HasOverCurrent();

private:
    static constexpr uint8_t hsd_count = 2;
    static constexpr uint8_t channels_per_hsd[] = {4, 1};

    static constexpr uint8_t channel_count = 5;

    HSD_Base* _hsds[hsd_count];
    Reading _channels[channel_count] = {};
};

}  // namespace macfe::fc

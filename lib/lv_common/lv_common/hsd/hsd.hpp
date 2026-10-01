/// @author Manush Patel
/// @date 2026-05-23

#pragma once
#include <cstdint>

#include "generated/can/veh_bus.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace macfe::lv {

using macfe::periph::AnalogInput;
struct Reading {
    float current_ma = 0.0f;
    bool fault = false;
};
// named to avoid conflict with overarching HSD class.
// depending on future direction, this can be made into a template that uses
// std::reference_wrapper for an array of n sel pins

class HSD_Base {
public:
    virtual Reading Read(uint8_t channel) = 0;
};
class HSD2 : public HSD_Base {
public:
    HSD2(macfe::periph::AnalogInput& isense,
         macfe::periph::DigitalOutput& isense_en,
         macfe::periph::DigitalOutput& sel1)
        : _isense(isense), _isense_en(isense_en), _sel1(sel1) {};
    Reading Read(uint8_t channel) override;

private:
    macfe::periph::AnalogInput& _isense;
    macfe::periph::DigitalOutput& _isense_en;
    macfe::periph::DigitalOutput& _sel1;
    float prev_tick_ch[2] = {};
};

class HSD4 : public HSD_Base {
public:
    HSD4(macfe::periph::AnalogInput& isense,
         macfe::periph::DigitalOutput& isense_en,
         macfe::periph::DigitalOutput& sel1, macfe::periph::DigitalOutput& sel2)
        : _isense(isense), _isense_en(isense_en), _sel1(sel1), _sel2(sel2) {};
    Reading Read(uint8_t channel) override;

private:
    macfe::periph::AnalogInput& _isense;
    macfe::periph::DigitalOutput& _isense_en;
    macfe::periph::DigitalOutput& _sel1;
    macfe::periph::DigitalOutput& _sel2;
    float prev_tick_ch[4] = {};
};
class HSD {
public:
    HSD(HSD_Base& hsd1, HSD_Base& hsd2, HSD_Base& hsd3, HSD_Base& hsd4,
        HSD_Base& hsd5, HSD_Base& hsd6)
        : _hsds{&hsd1, &hsd2, &hsd3, &hsd4, &hsd5, &hsd6} {}

    void Update_10Hz(generated::can::VehBus& bus);

    bool HasOverCurrent();

private:
    static constexpr uint8_t hsd_count = 6;
    static constexpr uint8_t channels_per_hsd[] = {4, 2, 2, 2, 2, 2};

    static constexpr uint8_t channel_count = 14;

    HSD_Base* _hsds[hsd_count];
    Reading _channels[channel_count] = {};
};

}  // namespace macfe::lv

#pragma once
#include "bindings.hpp"

struct Channel {
    float current_amps = 0.f;
    bool overcurrent = false;
};
namespace hsd {
static float ReadChannel2(AnalogInput& isense, DigitalOutput& isense_en,
                          DigitalOutput& sel, bool sel_val);
static float ReadChannel4(AnalogInput& isense, DigitalOutput& isense_en,
                          DigitalOutput& sel0, DigitalOutput& sel1, bool s0,
                          bool s1);
}  // namespace hsd
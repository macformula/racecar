#include "hsd.hpp"

#include "bindings.hpp"

namespace hsd {

static float ReadChannel2(AnalogInput& isense, DigitalOutput& isense_en,
                          DigitalOutput& sel, bool sel_val) {
    sel.Set(sel_val);
    isense_en.SetHigh();
    float v = isense.ReadVoltage();
    isense_en.SetLow();
    return v;
}

static float ReadChannel4(AnalogInput& isense, DigitalOutput& isense_en,
                          DigitalOutput& sel0, DigitalOutput& sel1, bool s0,
                          bool s1) {
    sel0.Set(s0);
    sel1.Set(s1);
    isense_en.SetHigh();
    float v = isense.ReadVoltage();
    isense_en.SetLow();
    return v;
}

}  // namespace hsd
/// @author Manush Patel
/// @date 2026-05-23

#include "hsd.hpp"

#include "alerts/alerts.hpp"
#include "bindings.hpp"

namespace hsd {

static constexpr float kVoltstoMa = 1400.0f / 0.5f;
static constexpr float kFaultThresholdV = 0.5f;

Reading::HSD1Channel1() {
    sel_.Set(0);
    en_.SetHigh();
    float v = isense_.ReadVoltage();

    en_.SetLow();

    return Reading {
        .current_ma = v * kVoltstoMa;
        .fault = (v > kFaultThresholdV);
    }
}

}  // namespace hsd
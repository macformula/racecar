/// @author Manush Patel
/// @date 2026-05-23

#include "hsd.hpp"

#include "alerts/alerts.hpp"
#include "bindings.hpp"

namespace hsd {

static constexpr float kVoltstoMa = 1400.0f / 0.5f;
static constexpr float kFaultThresholdV = 0.5f;

static constexpr uint8_t kTotalChannels = 5;
static Reading channels[kTotalChannels];

Reading HSD1Channel::Read(uint8_t channel) {
    en_.SetHigh();
    float v = isense_.ReadVoltage();
    en_.SetLow();

    return Reading {
        .current_ma = v * kVoltstoMa;
        .fault = (v > kFaultThresholdV);
    };
}

Reading HSD4Channel::Read(uint8_t channel) {
    sel0_.Set(channel & 0b01);
    sel1_.Set(channel & 0b10);

    en_.SetHigh();
    float v = isense_.ReadVoltage();

    en_.SetLow();

    return Reading{.current_ma = v * kVoltstoMa,
                   .fault = (v > kFaultThresholdV)};
}

bool HasOverCurrent() {
    for (const Reading& r : channels) {
        if (r.fault) {
            return true;
        }
    }
    return false;
}

void Update_10Hz(generated::can::VehBus&) {
    // HSD1 - 4 Channels
    channels[0] = bindings::hsd1.Read(0);
    channels[1] = bindings::hsd1.Read(1);
    channels[2] = bindings::hsd1.Read(2);
    channels[3] = bindings::hsd1.Read(3);

    channels[4] = bindings::hsd2.Read(
        0);  //  function requires channel but ideally should not

    bool any_fault = HasOverCurrent();

    alerts::Get().hsd_overcurrent = any_fault;
}

bool IsOverCurrent(uint8_t channel_index) {
    if (channel_index >= kTotalChannels) return false;
    return channels[channel_index].fault;
}

float GetCurrent(uint8_t channel_index) {
    if (channel_index >= kTotalChannels) return 0.0f;

    return channels[channel_index].current_ma;
}

}  // namespace hsd
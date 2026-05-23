#include "hsd.hpp"

#include "bindings.hpp"

namespace hsd {

static constexpr float kVoltstoMa = 1400.0f / 0.5f;
static constexpr float kFaultThresholdV = 0.5f;

Reading HSD2Channel::Read(uint8_t channel) {
    sel_.Set(channel & 0b1);
    en_.SetHigh();
    float v = isense_.ReadVoltage();

    en_.SetLow();

    return Reading {
        .current_ma = v * kVoltstoMa;
        .fault = (v > kFaultThresholdV),
    };
}

Reading HSD4Channel::Read(uint8_t channel) {
    sel0_.Set(channel & 0b01);
    sel1_.Set(channel & 0b10);

    en_.setHigh();
    float v = isense_.ReadVoltage();
    en_.setLow();

    return Reading {
        .current_ma = v * kVoltstoMa;
        .fault = (v > kFaultThresholdV),
    };
}

static constexpr uint8_t kTotalChannels = 13;
static Reading channels[kTotalChannels];

void Update_10Hz() {
    // HSD1 - 4 channels
    channels[0] = bindings::hsd1.Read(0);
    channels[1] = bindings::hsd1.Read(1);
    channels[2] = bindings::hsd1.Read(2);
    channels[3] = bindings::hsd1.Read(3);

    // HSD2 through HSD5 — 2 channels each
    channels[4] = bindings::hsd2.Read(0);
    channels[5] = bindings::hsd2.Read(1);

    channels[6] = bindings::hsd3.Read(0);
    channels[7] = bindings::hsd3.Read(1);

    channels[8] = bindings::hsd4.Read(0);
    channels[9] = bindings::hsd4.Read(1);

    channels[10] = bindings::hsd5.Read(0);
    channels[11] = bindings::hsd5.Read(1);

    // HSD6 — 1 channel
    channels[12] = bindings::hsd6.Read(0);
}

bool HasOverCurrent() {
    for (const Reading& r : channel) {
        if (r.fault) {
            return true;
        }
    }
    return false;
}

bool IsOverCurrent(uint8_t channel_index) {
    if (channel_index >= kTotalChannels) return false;

    return channel[channel_index].fault;
}
}  // namespace hsd